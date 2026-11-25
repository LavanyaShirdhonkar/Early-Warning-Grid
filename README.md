# Autonomous Landslide & Flash-Flood Early Warning Grid (ALF-EWG)

**CIC-211 OOP assignment | C++17**

Console simulation of fictional telemetry stations, an illustrative hazard score, simulated alert classification, region prioritization, and binary incident logging.

> **Safety/accuracy:** Academic simulation only. Sample data and score formula are invented. No sensors, sirens, emergency services, or real prediction model are connected. Never use for real safety decisions.

## OOP features demonstrated
- Encapsulation and validation; inline/overloaded functions; namespaces
- Friend auditor; dynamic array/deep copy/assignment/destructor
- Static station counter/network alert level
- Operators `+`, `>`, `<<`, `>>`
- Abstract base, runtime polymorphism, virtual and hybrid inheritance
- Composition (`PowerSubsystem`), exceptions and try/catch
- Binary persistence (`incidents.bin`)
- STL vector/map/sort/functor and custom template `PriorityQueue<T, Compare>`
- Startup self-checks (formula, cap, invalid input, exception catch, template ordering)

## Build
CMake:
```bash
cmake -S . -B build
cmake --build build
```
Run: Windows `build\\early_warning_grid.exe`; Linux/macOS `./build/early_warning_grid`.

Direct g++:
```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o early_warning_grid
```
The program appends records to `incidents.bin`; delete it to reset the count.

## Structure
```
Early-Warning-Grid/
├── .github/workflows/build.yml
├── .gitignore
├── CMakeLists.txt
├── README.md
├── docs/TECHNICAL_REPORT.md
└── src/main.cpp
```
GitHub Actions builds and runs the self-checks on pushes and pull requests. Review the Actions tab for run results. The self-checks are not a full test suite.

See [Technical Report](docs/TECHNICAL_REPORT.md) for design, syllabus mapping, test plan, limitations, and future work.
