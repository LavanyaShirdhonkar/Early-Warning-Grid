# Technical Report — ALF-EWG

**Course:** CIC-211 Object-Oriented Programming | **Language:** C++17 | **Type:** Console simulation

## 1. Abstract
ALF-EWG simulates environmental telemetry processing. Fictional stations receive sample values, calculate a demonstration score, print an alert classification and siren simulation, prioritize regions, and append incident records.

## 2. Problem and Objectives
The project illustrates how software could represent rainfall, soil saturation, and river-level readings for a classroom monitoring workflow. Objectives: encapsulate stations, demonstrate overloaded score functions, validate sample values, classify alerts, apply OOP concepts from Units I–IV, sort regional summaries, and save records.

## 3. Scope disclaimer
Readings, formula, and thresholds are invented for programming demonstration. The project is not connected to hardware, sirens, emergency services, or validated forecasting. It must not be used for real-world safety decisions. Operational systems require experts, validated data/models, calibration, official protocols, redundancy, field tests, and safety review.

## 4. Workflow
1. Create three fictional stations.
2. A polymorphic probe provides deterministic sample readings.
3. Validate station updates and compute score.
4. Map score to Normal, Watch, Warning, or simulated Evacuation Alert.
5. Print siren simulation; friend auditor displays sample values.
6. Append incident record to `incidents.bin`.
7. Sort regional summaries with the custom template queue.
8. Load stored records and display count.

## 5. Score formula
Main demo overload uses: `score = min(100, 0.30 × rainfall + 12 × riverLevel + 0.40 × soilSaturation)`. Weights are arbitrary, not scientific.

Illustrative categories: 0–29 Normal; 30–49 Watch; 50–69 Warning; 70–100 Evacuation Alert (simulation only).

## 6. OOP Architecture
- **SensorStation:** encapsulated ID, region, coordinates, readings and score; validates finite/non-negative values and soil range.
- **TelemetryBuffer:** dynamically allocated array, deep-copy constructor, copy assignment, bounds checks and destructor.
- **TelemetryAuditor:** friend class for demonstration access to private values.
- **DisasterTelemetryDevice:** abstract interface.
- **SolarPoweredNode / WirelessLoRaTransceiver:** virtual inheritance branches.
- **AutonomousHybridProbe:** hybrid derived class, implements virtual methods.
- **PowerSubsystem:** composed member.
- **EarlyWarningSiren:** alert-level class and static network maximum.
- **RegionCluster:** operator+ and operator>.
- **PriorityQueue<T, Compare>:** beginner-friendly user-defined template sorting items by a comparator.

## 7. CIC-211 Mapping
**Unit I:** namespaces, inline functions, function overloading, reference-returning normalization, friend class.  
**Unit II:** encapsulation/accessors, static members, dynamic memory/deep copy, constructor/assignment/destructor, operator overloads.  
**Unit III:** abstract class, pure virtual methods, virtual inheritance, hybrid inheritance, runtime polymorphism, composition.  
**Unit IV:** exception classes/try-catch, binary file I/O, vector/map/sort, functor, template class, custom stream manipulator.

## 8. Data Structures and Files
- `vector<SensorStation>`: station records
- `vector<BaseSensorNode*>`: polymorphic node references
- `map<string, RegionCluster>`: region summaries
- `PriorityQueue<RegionCluster, EvacuationPriority>`: sorted regions
- `vector<IncidentRecord>`: loaded records
- `incidents.bin`: append-only binary log

The binary struct format is a simple same-build classroom demonstration, not portable/versioned archival storage.

## 9. Verification
Startup self-checks test score cap, formula arithmetic, rejection of negative rainfall, catching the checksum exception type, and template queue ordering. **The checksum exception test only demonstrates catching that exception; no checksum algorithm is implemented.** GitHub Actions is configured to compile and run the program. This report does not claim CI has passed until a successful run is visible in Actions.

Manual checks: run twice to confirm stored count increases; delete `incidents.bin` to reset; inspect CI logs and output.

## 10. Build
```bash
cmake -S . -B build
cmake --build build
```
Or:
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o early_warning_grid
```

## 11. Limitations and Future Work
Fixed sample data; unvalidated formula/thresholds; checksum exception without checksum implementation; simple unversioned binary persistence; basic queue prioritizes clarity over performance. Future improvements: separate unit tests, timestamped inputs, file format versioning, user-input mode, and dashboard. Any real use would need extensive domain, engineering, and safety validation.

## 12. Conclusion
ALF-EWG demonstrates C++ OOP and STL concepts through a simple simulated monitoring workflow. It is intended for academic demonstration only.
