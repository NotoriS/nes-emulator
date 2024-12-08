﻿# CMakeList.txt : CMake project for nes-emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("nes-emulator")

# Add source to this project's executable.
add_executable (nes-emulator "src/nes-emulator.cpp" "src/nes-emulator.h" "src/cpu/cpu-common.cpp" "src/cpu/cpu.h" "src/main.cpp" "src/interfaces/i-bus.h" "src/cpu/cpu-opcode-switch.cpp" "src/cpu/cpu-bus.h" "src/cpu/cpu-bus.cpp" "src/cartridge/cartridge.h" "src/cartridge/cartridge.cpp"   "src/cartridge/mappers/mapper.h"  "src/cartridge/mappers/mapper-000.h" "src/cartridge/mappers/mapper-000.cpp" "src/cpu/cpu-instruction-fragments.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nes-emulator PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
