﻿# CMakeList.txt : CMake project for nes-emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("nes-emulator")

# Ensure consistent runtime library
if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "/MT")
  set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
endif()

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
    "src/ppu/colour-palette.h"
    "src/debug/logger.h"
    "src/debug/logger.cpp"
    "src/cpu/cpu-micro-instructions.cpp"
    "src/nes.h"
    "src/nes.cpp"
    "src/audio/apu.h" 
    "src/audio/apu.cpp"
    "src/audio/pulse-wave-generator.h" 
    "src/audio/pulse-wave-generator.cpp" 
    "src/audio/audio-utils.h" 
    "src/audio/audio-utils.cpp"
    "src/audio/frame-counter.h" 
    "src/audio/frame-counter.cpp" 
    "src/audio/audio-constants.h"
    "src/audio/length-counter-lookup.h"
    "src/audio/envelope.h" 
    "src/audio/envelope.cpp"
    "src/audio/triangle-wave-generator.h" 
    "src/audio/triangle-wave-generator.cpp"
    "src/audio/noise-generator.h" 
    "src/audio/noise-generator.cpp"
    "src/cartridge/mappers/mapper-002.h" 
    "src/cartridge/mappers/mapper-002.cpp"
    "src/cartridge/mappers/mapper-003.h" 
    "src/cartridge/mappers/mapper-003.cpp"
    "src/cartridge/mirror-mode.h"
    "src/cartridge/mappers/mapper-001.h" 
    "src/cartridge/mappers/mapper-001.cpp"
)

# Link SDL2
target_link_libraries(nes-emulator PRIVATE SDL2 SDL2main)

# Copy DLL dependencies to the executable directory on Windows
if (WIN32)
    add_custom_command(TARGET nes-emulator POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nes-emulator> $<TARGET_FILE_DIR:nes-emulator>
      COMMAND_EXPAND_LISTS
    )
endif()

set_property(TARGET nes-emulator PROPERTY CXX_STANDARD 20)

# ===== TESTS ======

# Add a test executable
add_executable(nes-emulator-tests
    "tests/cpu-tests.cpp"
    "src/cpu/cpu-common.cpp"
    "src/cpu/cpu.h"
    "src/interfaces/i-bus.h"
    "src/cpu/cpu-opcode-switch.cpp"
    "src/cpu/cpu-instructions.cpp"
    "tests/fakes/fake-cpu-bus.h"
    "src/debug/logger.h"
    "src/debug/logger.cpp"
    "src/cpu/cpu-micro-instructions.cpp"
)

# Link Google Test to the test executable
target_link_libraries(nes-emulator-tests PRIVATE gtest gtest_main)

enable_testing()
add_test(NAME NES_Emulator_Tests COMMAND nes-emulator-tests)

set_property(TARGET nes-emulator-tests PROPERTY CXX_STANDARD 20)
