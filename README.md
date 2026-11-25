# Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG)

A C++17 console simulation for the CIC-211 Object-Oriented Programming assignment. It models telemetry stations, computes an illustrative hazard index, classifies alerts, simulates regional siren dispatch, sorts region priorities, and stores incident records in a binary file.

> **Safety and accuracy:** Educational simulation only. It uses fixed sample readings and illustrative formulas. It is not connected to real sensors, sirens, emergency services, or validated disaster prediction models. Do not use it for real safety decisions.

## Features
- Encapsulated sensor stations with validation
- Overloaded and inline hazard-index functions
- Namespaces `DisasterCore::Sensors` and `DisasterCore::Alerts`
- Friend auditor to inspect private telemetry
- Dynamic telemetry buffer with deep copy, copy assignment, and destructor
- Static active-station counter and network alert level
- Operator overloads: `+`, `>`, `<<`, `>>`
- Abstract sensor node, virtual inheritance diamond, and runtime polymorphism
- Containership using `PowerSubsystem`
- Custom exception types
- Binary incident logging and STL containers/algorithms
- Custom comparator functor and stream manipulator

## Build and run
### Windows (MinGW g++)
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o early_warning_grid.exe
early_warning_grid.exe
```
### Linux/macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o early_warning_grid
./early_warning_grid
```
The program appends incident records to `incidents.bin` in the current working directory. Remove that file to reset the demo log.

## Repository layout
```
Early-Warning-Grid/
├── README.md
├── src/main.cpp
└── docs/TECHNICAL_REPORT.md
```
See [Technical Report](docs/TECHNICAL_REPORT.md) for architecture, workflow, and mapping to CIC-211 Units I–IV.
