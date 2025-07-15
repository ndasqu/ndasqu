c300_systemc/
├── core/
│   ├── c300_core.cpp
│   ├── c300_core.hpp
│   ├── core_security.hpp
│   └── core_doc.md
│
├── engine/
│   ├── c300_engine.cpp
│   ├── c300_engine.hpp
│   └── engine_doc.md
│
├── controller/
│   ├── c300_controller.cpp
│   ├── c300_controller.hpp
│   └── controller_doc.md
│
├── network/
│   ├── c300_network.cpp
│   ├── c300_network.hpp
│   └── network_doc.md
│
├── system/
│   ├── c300_system.cpp
│   ├── c300_system.hpp
│   └── system_doc.md
│
├── synthesis/
│   ├── constraints/       ← Timing constraints
│   ├── tcl/              ← Synthesis scripts
│   └── verification/     ← Testbench files
│
├── build/
│   ├── lib/              ← SystemC .so files
│   └── bin/              ← Executable files
│
├── CMakeLists.txt
├── Makefile
└── README.md
