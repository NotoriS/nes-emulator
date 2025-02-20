# NES Emulator

This emulator replicates the functionality of the classic Nintendo Entertainment System (NES). The project focuses on achieving high accuracy while adhering to clean code practices. The project aims to provide support for emulating all official NES games.

## Table of Contents
- [Screenshots](#screenshots)
- [Outline and Progress](#outline-and-progress)
- [Goals](#goals)

## Screenshots

![Screenshot 2025-02-20 161715](https://github.com/user-attachments/assets/61942b6f-8f9f-4308-88f4-a37aff9ff5b9)
![Screenshot 2025-02-20 162329](https://github.com/user-attachments/assets/8476395a-06e1-433b-83c9-587697cf5aee)

## Outline and Progress

### Stage 1: Planning and Project Architecture
- Design an outline for programming modules the emulate the CPU, main bus, memory, and game cartridge.

**Progress:** Completed

### Stage 2: CPU Interface, Main Bus, and Memory
- Implement the main bus for communication between the CPU, memory, and other components.
- Establish the CPU interface to interact with the main program or other components.

**Progress:** Completed

### Stage 3: ROM Loading and Mapper 0
- Implement the ability to load NES ROM files into the emulator.
- Add support for Mapper 0 (the simplest ROM type), handling basic ROM header parsing and memory mapping.

**Progress:** Completed

### Stage 4: CPU Implementation
- Emulate the 6502 CPU and implemented the key instructions, addressing both logic and arithmetic functions.
- Break down each instruction into it's individual clock cycles, managing the CPU state using a queue of lambda expressions
- Provide public methods for performing hardware interrupts
  
**Progress:** Completed

### Stage 5: User Input System
- Add support for user input using the SDL library.

**Progress:** Completed

### Stage 6: PPU Implementation
- Start implementing the Picture Processing Unit (PPU) to handle graphics rendering.
- Focus on replicating the behavior of the NES’s graphics pipeline, including background layers, sprites, and color palettes.

**Progress:** Completed

### Stage 7: APU Implementation
- Set up the Audio Processing Unit (APU) for sound generation, aiming to support the basic channels for sound effects and music.

**Progress:** Planned

### Stage 8: Additional Mappers for Extended Game Support
- Add support for additional mappers to handle more complex ROMs.
- Ensure compatibility with a wider range of NES games that use different mappers for memory management.

**Progress:** Planned

## Goals

1. **Accuracy**
   - The emulator aims to replicate the NES hardware as accurately as possible, with particular attention to the behavior of the 6502 CPU, the PPU’s rendering pipeline, and the APU's sound channels. The goal is to ensure that games run faithfully to their original versions.

2. **Clean Code**
   - The codebase follows best practices for writing clean, modular, and maintainable code. This involves clear naming conventions, well-defined functions, and consistent coding style, making it easy to contribute to and extend the project.

3. **Functional Programming**
   - The CPU implementation is being developed using functional programming principles. In order to ensure that the CPU emulation is as accurate and readable as possible, the CPU state will be managed using a queue of method pointers. Each time a new opcode is read, the queue will be populated with single cycle micro-instructions that will be dequeued each time the CPU is clocked.
