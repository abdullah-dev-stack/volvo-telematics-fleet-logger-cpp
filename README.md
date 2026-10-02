# Volvo Telematics Fleet Logger

🚀 **C++** | **Object-Oriented Programming** | **SQLite** | **Vehicle Telemetry** | **Automated Safety Monitoring**

An independent educational project that simulates a vehicle fleet telematics system using C++, Object-Oriented Programming (OOP), and SQLite.

---

## 📋 Overview

**Volvo Telematics Fleet Logger** is a C++-based vehicle telemetry simulation designed to demonstrate how an automotive system can collect, process, store, and monitor vehicle sensor data.

The system simulates a vehicle continuously generating live telemetry parameters:
*   **Vehicle speed**
*   **Engine temperature**
*   **Fuel level**
*   **Battery voltage**

The generated data is persisted in a local SQLite database. The system also performs automatic safety checks and creates alerts when potentially dangerous conditions are detected. 

This project demonstrates the connection between vehicle sensor simulation, core application logic, database persistence, and automated safety monitoring.

> ⚠️ **Important:** This is an independent student project and is not an official Volvo Group product, system, or software. The project name and concept are used solely for educational purposes to demonstrate automotive/telematics engineering concepts.

---

## 🎯 Project Objective

The main objective of this project is to build a realistic simulation of a fleet telematics logger. 

The system demonstrates how a connected vehicle can:
1. Generate real-time sensor readings dynamically.
2. Send telemetry data securely to a localized processing layer.
3. Store the structured readings in an embedded database.
4. Evaluate multi-variable safety thresholds.
5. Automatically trigger emergency alerts.
6. Persist those alerts reliably for later diagnostic analysis.

---

## 🏗️ Core Architecture

```text
┌──────────────────────┐
│   Simulated Vehicle  │
│      Sensors         │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│    EngineSensor      │
│   C++ Simulation     │
└──────────┬───────────┘
           │
           │ Telemetry
           ▼
┌──────────────────────┐
│    TelematicsDB      │
│   Database Layer     │
└──────────┬───────────┘
           │
           ├──────────────► SensorLogs (Table)
           │
           └──────────────► Safety Checks (Logic)
                                  │
                                  ▼
                           ┌──────────────┐
                           │    Alerts    │
                           └──────────────┘
```

---

## ✨ Key Features

### 1. Vehicle Telemetry Simulation
The application generates dynamic, random readings for a virtual vehicle infrastructure:
*   **Vehicle ID:** `VG-8492`

| Sensor Field | Simulated Range |
| :--- | :--- |
| **Speed** | 60 – 84 km/h |
| **Engine Temperature** | 90 – 109 °C |
| **Fuel Level** | 10% – 100% |
| **Battery Voltage** | 11.0V – 14.5V |

### 2. Continuous Telemetry Streams
The system performs multiple sequential telemetry cycles to imitate a physical vehicle sending data packets over a time horizon:
`Tick 1` ➔ `Read sensors` ➔ `Store data` ➔ `Check safety rules`

A explicit **1-second delay** is enforced between cycles using multi-threading to closely mirror a true hardware telemetry stream.

### 3. SQLite Database Integration
The project integrates the raw SQLite C-API as its local transactional persistence layer:
*   **Database File:** `fleet_data.db`
*   The system utilizes robust initialization queries to automatically generate relational database tables if they do not exist at startup.

#### Schema Profiles
*   **`SensorLogs`:** Stores physical vehicle stats.
    *   `LogID` (Primary Key), `VehicleID`, `Speed`, `EngineTemp`, `FuelLevel`, `BatteryVoltage`, `Timestamp`.
*   **`Alerts`:** Stores automatically triggered system exceptions.
    *   `AlertID` (Primary Key), `VehicleID`, `WarningMessage`, `Timestamp`.

---

## 🚨 Automated Safety Monitoring

The application processes telemetry data in real-time, executing logical evaluations against predefined physical safe operating bounds:

*   **Engine Overheating**
    *   **Rule:** If Engine Temperature > 105°C
    *   *Output:* `CRITICAL: Engine Overheating! Temperature at 108 C`
*   **Low Fuel Level**
    *   **Rule:** If Fuel Level < 15%
    *   *Output:* `WARNING: Low Fuel Level! Fuel at 12%`
*   **Low Battery Voltage**
    *   **Rule:** If Battery Voltage < 11.5V
    *   *Output:* `WARNING: Low Battery Voltage! Voltage at 11.2V`

### 🔒 Database Safety (Prepared Statements)
To maintain defensive coding standards, the project relies entirely on **SQLite prepared statements** (`sqlite3_prepare_v2`) and parameter binding (`sqlite3_bind_double`, `sqlite3_bind_text`). This prevents unsafe SQL construction patterns and ensures optimized query plan executions.

---

## 🧩 Object-Oriented Design

The codebase relies heavily on clean separation of concerns across two primary classes:

### `TelematicsDB`
Manages the structural data persistence layer.
*   Handles connections, opens/closes database pointers (`sqlite3*`).
*   Runs table creation scripts and structured queries.
*   Performs structural safety rule evaluations and binds execution parameters.
*   Queries historical database structures to generate post-simulation metrics.

### `EngineSensor`
Manages the hardware-level vehicle simulation logic.
*   Maintains state models for the mock vehicle identification and timing sequences.
*   Spawns pseudo-random distributions to simulate actual telemetry noise.
*   Coordinates the telemetry sleep cycles and pushes runtime states directly to the database controller.

---

## 🛠️ Technology Stack

| Component | Technology | Utilization Purpose |
| :--- | :--- | :--- |
| **Language** | C++ | Core systems application backend logic |
| **Architecture** | OOP | Domain encapsulation and class-based design |
| **Database** | SQLite3 | Embedded local transactional persistence layer |
| **API Boundary** | SQLite C API | Low-level C database communications |
| **Randomization** | `<random>` | Generating realistic vehicle tracking data variations |
| **Concurrency** | `<thread>` / `<chrono>` | Implementing 1-second telemetry interval timing steps |
| **VCS** | Git / GitHub | Code management, version history, and portfolio presentation |

---

## 🧠 C++ Concepts Demonstrated

*   **Encapsulation:** Protecting data access bounds inside specialized class boundaries using `public` and `private` keywords.
*   **Object Composition:** Managing a runtime communication workflow where `EngineSensor` coordinates directly with instance pointers of `TelematicsDB`.
*   **Resource Management (RAII):** Utilizing constructors for automated database setup and destructors for guaranteed database connection closing (`sqlite3_close`).
*   **Advanced Memory Manipulation:** Working with raw pointers safely during interaction bindings with external database structures.

---

## 📂 Project Structure

```text
Volvo-Telematics-Fleet-Logger/
│
├── main.cpp               # Main execution logic, simulation thread, and class declarations
├── sqlite3.h              # SQLite embedded header core library 
├── sqlite3.c              # SQLite translation source file
├── README.md              # Project documentation and engineering breakdown
├── .gitignore             # Standard file ignore definitions
└── fleet_data.db          # Auto-generated database file (Excluded from version control)
```

---

## 💻 Example Program Flow

When the console binary boots up:

```text
======================================================
 VEHICLE FLEET TELEMATICS & ALERT SIMULATOR
 C++ / OOP / SQLite
======================================================

[Database] SQLite database connected successfully.
[Sensor] Starting engine telematics for Vehicle: VG-8492

[Tick 0] Reading vehicle sensors...
[Database] Logged: Speed 71 km/h | Engine Temp 78 C | Fuel Level 41% | Battery Voltage 11.0214 V
>> [SYSTEM ALERT] WARNING : Low Battery | Voltage at 11.0214 V

...

[Tick 7] Reading vehicle sensors...
[Database] Logged: Speed 68 km/h | Engine Temp 81 C | Fuel Level 92% | Battery Voltage 13.2802 V
[Sensor] Engine shut down.
[Sensor] Simulation Complete.

======================================================
 [Database Summary]
======================================================
Total Sensor readings stored: 8
Total Alerts stored: 3
Simulation Complete. Exiting Program.
```

---

## 📈 Real-World Perspective

In a industrial telemetry landscape, heavy vehicles generate large scale operational data metrics parsed from the vehicle's physical internal CAN Bus. This simulation models that behavior locally:

```text
Vehicle ➔ Physical Sensors ➔ CAN Bus Transceiver ➔ Telematics Unit ➔ Edge Database ➔ Enterprise Cloud Routing ➔ Live Monitoring Dashboard
```
Instead of target deployment onto live automotive hardware, this simulation replicates the process deterministically inside local runtime space using **SQLite**.

---

## 🚀 Future Roadmap

To scale this infrastructure into an enterprise-ready fleet logistics platform, future developments include:
*   **Multi-Vehicle tracking:** Simulating multiple independent vehicles processing data lines concurrently.
*   **Geospatial Tracking:** Integrating mock GPS longitude and latitude strings into log streams.
*   **Expanded Telemetry Matrix:** Simulating tire pressure indexes, brake pad thermal wear, and diagnostic trouble codes (DTC).
*   **Export Pipeline:** Writing operational metrics directly out into `.csv` or `.json` structures for external ingestion layers.

---

# ⚙️ Build and Run Instructions

### Prerequisites
*   A functional C++ Compiler (`g++`)
*   Local terminal environment with Git accessibility

### Compiling via CLI
Since the external SQLite package is embedded as a single-source configuration file directly inside the source workspace, you should compile the source architectures together:

```powershell
# Compile the SQLite object binary individually using a C standard layout
gcc -c sqlite3.c -o sqlite3.o

# Compile and bind your core application layers with the C object structure
g++ main.cpp sqlite3.o -o volvo-telematics-logger.exe
```

### Execution
Run the compiled application matching your native execution environment:

```powershell
# Windows
.\volvo-telematics-logger.exe

# Linux / macOS
./volvo-telematics-logger.exe
```

---

## 👨‍💻 Author

**Abdullah Khan**  
### Areas of Engineering Interest:
*   **Systems Architecture & C++ Development**
*   **Database Engine Internals** (MySQL, PostgreSQL, SQLite)
*   **Automotive Systems Engineering & Telematics**
*   **Distributed Systems, Networks & IoT**

---

## ⚖️ License & Disclaimer

This project is licensed under the terms of the **MIT License**.

> ⚠️ **Disclaimer:** This system is a completely independent, non-commercial software simulation engineered exclusively for academic assessment. It is not affiliated with, sponsored by, or directly managed by Volvo Group or any of its subsidiaries.


*   This project is strictly an educational system model simulation.
*
