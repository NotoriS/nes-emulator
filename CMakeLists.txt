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

# Ensure consistent runtime library
set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

# Include external libraries
add_subdirectory(external/SDL)
add_subdirectory(external/googletest)

# Add source to this project's executable.
add_executable (nes-emulator
    "src/main.cpp"
    "src/cpu/cpu-common.cpp"
    "src/cpu/cpu.h"
    "src/interfaces/i-bus.h"
    "src/cpu/cpu-opcode-switch.cpp"
    "src/cpu/cpu-bus.h"
    "src/cpu/cpu-bus.cpp"
    "src/cartridge/cartridge.h"
    "src/cartridge/cartridge.cpp"
    "src/cartridge/mappers/mapper.h"
    "src/cartridge/mappers/mapper-000.h"
    "src/cartridge/mappers/mapper-000.cpp"
    "src/cpu/cpu-instructions.cpp"
    "src/ppu/ppu.h"
    "src/ppu/ppu.cpp"
    "src/ppu/ppu-bus.h"
    "src/ppu/ppu-bus.cpp"
)

# Link SDL2
target_link_libraries(nes-emulator PRIVATE SDL2 SDL2main)

# Copy DLL dependencies to the executable directory
add_custom_command(TARGET nes-emulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nes-emulator> $<TARGET_FILE_DIR:nes-emulator>
  COMMAND_EXPAND_LISTS
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nes-emulator PROPERTY CXX_STANDARD 20)
endif()

# ===== TESTS ======

# Add a test executable
add_executable(nes-emulator-tests
    "tests/cpu-tests.cpp"
    "src/cpu/cpu-common.cpp"
    "src/cpu/cpu.h"
    "src/interfaces/i-bus.h"
    "src/cpu/cpu-opcode-switch.cpp"
    "src/cartridge/cartridge.h"
    "src/cartridge/cartridge.cpp"
    "src/cartridge/mappers/mapper.h"
    "src/cartridge/mappers/mapper-000.h"
    "src/cartridge/mappers/mapper-000.cpp"
    "src/cpu/cpu-instructions.cpp"
    "tests/fakes/fake-cpu-bus.h"
)

# Link Google Test to the test executable
target_link_libraries(nes-emulator-tests PRIVATE gtest gtest_main)

enable_testing()
add_test(NAME NES_Emulator_Tests COMMAND nes-emulator-tests)
