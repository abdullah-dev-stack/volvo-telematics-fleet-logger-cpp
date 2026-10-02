#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#endif
#include "sqlite3.h"

using namespace std;


//===========================================================================
//==========================================

// ClASS 1 : Telematics Database 
// Responsible for sorting sensor data and system alerts 

class TelematicsDB {
private:
  sqlite3* db = nullptr;

  bool executeSQL(const char* sql) {
    char* errorMessage = nullptr;
    const int result = sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage);

    if (result != SQLITE_OK) {
      cerr << "SQL error: "
          << (errorMessage != nullptr ? errorMessage : sqlite3_errmsg(db))
          << endl;
      sqlite3_free(errorMessage);
      return false;
    }

    return true;
  }

  public:
      TelematicsDB() {
           
            if 
            (sqlite3_open("fleet_data.db", &db) != SQLITE_OK) {

              cerr << "Fsiled to opem SQLITE database: " << 
              sqlite3_errmsg(db) << endl;
              return;


            }
         cout  << "[Database] SQLite database connected successfully." 
         << endl ; 

         // Table for continous vehicl telemetry 
         const char* createlogs = 
         R"(   
           CREATE TABLE IF NOT EXISTS SensorsLogs (
           logID INTEGER PRIMARY KEY AUTOINCREMENT, 
           VehicleID TEXT NOT NULL , 
           Speed INTEGER NOT NULL ,
            EngineTemp INTEGER NOT NULL, 
            FuelLevel INTEGER NOT NULL, 
            BatteryVoltage REAL INTEGER NOT NULL,
            Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP 

            ); 

         )";
             
         // Table for critical system alerts 
         const char* createAllerts =
         R"(
             CREATE TABLE IF NOT EXISTS Alerts (
             AlertID INTEGER PRIMARY KEY AUTOINCREMENT , 
             VehicleID TEXT NOT NULL , 
             WarningMessage TEXT NOT NULL , 
             Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP

             );
         )";
         
         if (! executeSQL(createlogs)) {
            cerr << "Failed to create SensoreLoge table. " << endl;

         }

          if (! executeSQL(createAllerts)) {
            cerr << "Falied to create Alerts table. " << endl;

          }

          }

          ~TelematicsDB() {
            if (db != nullptr) {
              sqlite3_close(db);
            }
          }
        
         // 
         //===============================================
        
         // Store sensor readingd usings  prepared statements 
         // 

         void logSensorData(
            const std::string& vehicleID,
         int speed,
         int temperature,
         int fuelLevel,
         double batteryVoltage

         ) {

            const char* sql = R"sql(
              INSERT INTO SensorsLogs 
            (       VehicleID , Speed, EngineTemp, FuelLevel, BatteryVoltage)  
           
           
           VALUES (?,  ? , ? , ?,  ?);

           )sql";
         
           sqlite3_stmt* statement  = nullptr; 

           int result = sqlite3_prepare_v2(
               db,
               sql,
               -1,
               &statement, 
               nullptr 
           );
         
           if (result != SQLITE_OK) {
            cerr << "Failed to prepare sensor INSERT statement: " 
                 << sqlite3_errmsg(db) << endl;
            return;
           }
           // Bind values safely to SQL statement parameters 
           sqlite3_bind_text(
           statement ,
           1,
           vehicleID.c_str(),
           -1,
           SQLITE_TRANSIENT 

           );
         sqlite3_bind_int(statement , 2 , speed);
         sqlite3_bind_int(statement , 3 , temperature);
         sqlite3_bind_int(statement , 4 , fuelLevel );
         sqlite3_bind_double(statement , 5 , batteryVoltage);
          result = sqlite3_step(statement); 
          if (result != SQLITE_DONE) {
         cerr << "[Error] Failed to save sensor data: " 
          << sqlite3_errmsg(db) << endl;
          } else {
            cout << "[Database] Logged: "
                 << "Speed " << speed << " km/h, "
                 << "Engine temperature " << temperature << " C, "
                 << "Fuel level " << fuelLevel << "%, "
                 << "Battery voltage " << batteryVoltage << " V"
                 << endl;
          }

          sqlite3_finalize(statement);

          // Run automatic safety checks 
          checkSafetyConditions(
             vehicleID,
             temperature,
             fuelLevel,
             batteryVoltage            
          );
        }

            void triggerAlert(
                const std::string& vehicleID,
                const std::string& warningMessage
            ) {
              const char* sql = R"sql(
                INSERT INTO Alerts (VehicleID, WarningMessage)
                VALUES (?, ?);
              )sql";

              sqlite3_stmt* statement = nullptr;
              int result = sqlite3_prepare_v2(
                db, sql, -1, &statement, nullptr
              );

              if (result != SQLITE_OK) {
                cerr << "Failed to prepare alert INSERT statement: "
                   << sqlite3_errmsg(db) << endl;
                return;
              }

              sqlite3_bind_text(
                statement, 1, vehicleID.c_str(), -1, SQLITE_TRANSIENT
              );
              sqlite3_bind_text(
                statement, 2, warningMessage.c_str(), -1, SQLITE_TRANSIENT
              );

              result = sqlite3_step(statement);
              if (result != SQLITE_DONE) {
                cerr << "Failed to save alert: " << sqlite3_errmsg(db) << endl;
              } else {
                cout << "[Alert] " << warningMessage << endl;
              }

              sqlite3_finalize(statement);
            }

            // Automatic vehicle safety monitoring 
            //

            void checkSafetyConditions(
                    const std::string& vehicleID,
                    int temperature,
                    int fuelLevel,
                    double batteryVoltage
            )   {
                  
                if (temperature > 105) {
                  triggerAlert(
                    vehicleID,
                    "CRITICAL : Enginre Overheating ! Temperature at " + std::to_string(temperature) + " C"
                  );          

                }
               if (fuelLevel <15) {
                  triggerAlert(
                     vehicleID , 
                     "WARNING : Low ! Fuel at"
                         + std::to_string(fuelLevel) + "%"
                  );

               }
 
                if (batteryVoltage < 11.5) {
                  
                  triggerAlert(
                     vehicleID,
                     "WARNING : Low Battery ! Voltage at "
                         + std::to_string(batteryVoltage) + " V"
                  );

                }

            }
            
          void displayDatabaseSummary() {
              const char* countSQL = "SELECT COUNT(*) FROM SensorsLogs;";
              sqlite3_stmt* countStatement = nullptr;
              int countResult = sqlite3_prepare_v2(
                db, countSQL, -1, &countStatement, nullptr
              );
              if (countResult != SQLITE_OK) {
                cerr << "Failed to prepare count statement: "
                     << sqlite3_errmsg(db) << endl;
                return;
              }
              if (sqlite3_step(countStatement) == SQLITE_ROW) {
                cout << "[Database Summary]" << endl;
                cout << "Total Sensor readings stored: "
                     << sqlite3_column_int(countStatement, 0) << endl;
              }
              sqlite3_finalize(countStatement);
              displayTotalAlerts();
          }

          void displayTotalAlerts() {
                const char* countSQL = R"(
                  SELECT COUNT(*) FROM Alerts;
                )";
                sqlite3_stmt* countStatement = nullptr;
                int countResult = sqlite3_prepare_v2(
                  db,
                  countSQL,
                  -1,
                  &countStatement,
                  nullptr
                );
                if (countResult != SQLITE_OK) {
                  cerr << "Failed to prepare count statement: "
                       << sqlite3_errmsg(db) << endl;
                  return;
                }
                if (sqlite3_step(countStatement) == SQLITE_ROW) {
                  int totalAlerts = sqlite3_column_int(countStatement, 0);
                  cout << "[Database Summary]" << endl;
                  cout << "Total Alerts stored: " << totalAlerts << endl;
                }
                sqlite3_finalize(countStatement);
                
              }

};



//  ==========================================

//  ==========================================


// Class 2 : Engine Sensor Simulator 
// Generates simulated vehicle telemetry 
//
//==============
//===================


class EngineSensorSimulator {
    private : 
    string vehicleID;
    TelematicsDB* database ;
     
    // Modern C++ Random number generator
    
    random_device randomDevice;
    mt19937 generator;
    public : 
    EngineSensorSimulator(
        const std::string& vehicleID, TelematicsDB* db)
        : vehicleID(vehicleID), database(db), randomDevice(), generator(randomDevice())
    {
    }

    // Generate a random integer between two values 
    //
    int generateInteger(int minimun, int maximun) {
       uniform_int_distribution<int> distribution(
         minimun, maximun
      );
         return 
         distribution(generator);

    }

    // Generate a random floating-point value between two values
    double generateDouble(double minimum, double maximum) {
      uniform_real_distribution<double> distribution(minimum, maximum);
      return distribution(generator);
    }
        // 
           // Start simulated vehicle telemetry 
           
            // -----------------------------------------------
           // ----------------------
          void startEngineSimulation(int iteration) {
              cout << "\n [Sensor] Starting engine telematics "
              << "For Vehicle:"
              << vehicleID 
              << endl;

               cout << 
               "--------------------------------------------------"
               "----------------------------------------" << endl;

                for (int i = 0; i <=iteration; i++) {

                        // Simulated vehicle readings 
                        int currentspeed  = generateInteger(60 , 80);
                        int currentTemperature = generateInteger(60, 84 );
                        int currentFuelLevel = generateInteger(10, 100);
                        double currentBatteryVoltage = generateDouble(11.0, 14.5);
                        cout << "\n[Tick " << i << "] "
                             << "Speed: " << currentspeed << " km/h, "
                             << "Engine Temp: " << currentTemperature << " C, "
                             << "Fuel Level: " << currentFuelLevel << "%, "
                             << "Battery Voltage: " << currentBatteryVoltage << " V"
                             << endl;


                             // Send Telemetry to Database 
                             database->logSensorData(vehicleID, currentspeed, currentTemperature, currentFuelLevel, currentBatteryVoltage);
                               // Wait one second to simulate 
                               // continous Iot telemertry 
#ifdef _WIN32
                               Sleep(1000);
#else
                               std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
                         
                }
          
                 cout << "[Sensor] Engine shut down . "
                 << endl; 
                 cout << "[Sensor] Simulation Complete." 
                 << endl ;
            }

  };

  // Main Program 

  int main() {
     cout << "===============================" << endl;
        cout << "Volvo Telematics Logger  C++ " << endl;
        cout << "===============================" << endl;

        // Create Database object 
        TelematicsDB FleetDatabase;

        // Create Simulated Vehicle Sensor 
        EngineSensorSimulator volvoTruck("VG-8492", &FleetDatabase);

        // Run Simulation For 7 Telemetry Cycles 
        volvoTruck.startEngineSimulation(7);

        // Display Database Statistics 
         FleetDatabase.displayDatabaseSummary();
         FleetDatabase.displayTotalAlerts();
          cout << "===============================" << endl;
          cout << "Simulation Complete. Exiting Program." << endl;
          cout << "===============================" << endl;
           return 0;           

  }