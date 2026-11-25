# Technical Report — ALF-EWG

## 1. Project Overview
The Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG) is a console-based C++ simulation of a regional hazard-monitoring system. It represents sensor stations, telemetry processing, hazard scoring, alert classification, simulated siren dispatch, region prioritization, and persistent incident logging.

## 2. Problem Statement
Mountain regions can experience landslides and flash floods after combinations of heavy rainfall, soil saturation, slope instability, and rising river levels. Monitoring multiple indicators and maintaining incident records can help demonstrate how a software alert pipeline might support preparedness. This project implements a simplified educational model using simulated readings.

## 3. Objectives
- Represent telemetry stations with encapsulated state.
- Compute a composite hazard index from simulated environmental measurements.
- Assign alert levels and demonstrate alert dispatch.
- Apply CIC-211 concepts across Units I–IV.
- Persist incident records and sort regional priorities.

## 4. Scope and Assumptions
This prototype is a deterministic classroom simulation. It does not ingest real sensor data, connect to a network, control sirens, or provide validated hazard predictions. Its formula and thresholds are illustrative and must not be used for operational safety decisions. Real deployment would require domain experts, validated datasets, calibration, reliability engineering, official alert protocols, and redundant communications.

## 5. System Workflow
1. Create simulated sensor stations.
2. A polymorphic sensor node performs a simulated telemetry sweep.
3. Measurements are validated and a composite hazard score is computed.
4. A score is mapped to a demonstration alert level.
5. The alert object prints a siren-dispatch simulation.
6. The auditor prints private telemetry for academic demonstration.
7. A binary incident record is appended to disk.
8. Region records are sorted using a custom priority functor.
9. Previously logged records are counted and displayed.

## 6. Hazard Scoring
The code contains overloaded `computeHazardIndex()` functions for rainfall+slope, a typed soil-saturation+vibration telemetry structure, and rainfall+river-level+saturation. The demonstration uses the third overload. This hand-designed example formula is not a scientific model. Demo alert mapping:
- 0–29: Normal
- 30–49: Watch
- 50–69: Warning
- 70–100: Evacuation Alert (simulated)

## 7. Architecture and OOP Design
- **SensorStation:** encapsulates ID, region, coordinate, readings, and score.
- **TelemetryBuffer:** owns dynamically allocated values; deep-copy constructor, assignment, and destructor manage memory.
- **TelemetryAuditor:** friend class demonstrating controlled access to private raw telemetry.
- **DisasterTelemetryDevice:** virtual root interface.
- **SolarPoweredNode / WirelessLoRaTransceiver:** virtual intermediate inheritance branches.
- **AutonomousHybridProbe:** derived hybrid node; implements telemetry reading.
- **PowerSubsystem:** composed object owned by the sensor node.
- **EarlyWarningSiren:** encapsulates alert level; static network-wide maximum alert.
- **RegionCluster:** overloads `+` to combine cluster statistics and `>` for comparisons.

## 8. CIC-211 Syllabus Mapping

### Unit I — Foundations and Call Semantics
- Namespaces: `DisasterCore::Sensors` and `DisasterCore::Alerts`.
- Inline hazard-index calculation functions.
- Function overloading for different telemetry parameter combinations.
- Coordinate normalization returns a reference; station update modifies state.
- Friend class `TelemetryAuditor` accesses private sensor fields.

### Unit II — Encapsulation, Dynamic Memory, Operator Overloading
- Encapsulation and const-correct accessors in `SensorStation`.
- Static station count and global alert level.
- Dynamic array in `TelemetryBuffer`; parameterized constructor, deep-copy constructor, copy assignment, destructor.
- `RegionCluster::operator+`, `operator>`, and `SensorStation` stream operators.

### Unit III — Inheritance, Containership, Polymorphism
- Abstract `BaseSensorNode` declares pure virtual `readTelemetry()` and `evaluateThreshold()`.
- Virtual inheritance resolves the diamond through `DisasterTelemetryDevice`.
- `PowerSubsystem` is composed within the base node.
- A `std::vector<BaseSensorNode*>` dispatches methods at runtime.

### Unit IV — Streams, Persistence, Exceptions, STL
- Custom exceptions: sensor failure, checksum mismatch, threshold overflow.
- Binary `fstream` persistence for incident records.
- STL `vector`, `map`, `sort`, and custom comparator/functor.
- Custom stream manipulator `alertTable()` prints table headings.

## 9. Data Structures and File Storage
- `vector<SensorStation>`: station collection.
- `vector<BaseSensorNode*>`: polymorphic nodes.
- `map<string, RegionCluster>`: regional aggregation.
- `vector<IncidentRecord>`: loaded incident records.
- `incidents.bin`: append-only binary incident history in the working directory.

## 10. Build and Test Procedure
Compile using a C++17 compiler:
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o early_warning_grid
./early_warning_grid
```
Expected demonstration: simulated alert output, audit readings, station table, sorted region priority, active station count, and persisted record count. Run it twice to verify that the stored count increases. Delete `incidents.bin` to reset.

## 11. Limitations and Future Enhancements
- Replace fixed readings with validated sensor inputs or a clearly labeled dataset.
- Use a scientifically validated model and calibrated thresholds.
- Add checksum validation and safer transactional file replacement.
- Add unit tests, telemetry timestamps, sensor-health monitoring, and structured logging.
- Add a graphical dashboard/map as a separate extension.
- Establish secure communication, redundancy, fail-safe procedures, and expert review before any real deployment.

## 12. Conclusion
ALF-EWG demonstrates a modular OOP simulation of telemetry monitoring and alert classification while mapping major CIC-211 mechanisms to concrete code. It is intended for learning, presentation, and code review—not real disaster warning.
