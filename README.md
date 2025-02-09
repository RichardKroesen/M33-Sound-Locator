# Sound Locator M33 
This project has an educational purpose in the context of Digital Signal Processing & Control Systems. It is part of the Linear Systems (5LIU0)course at Technical University Eindhoven (TUe).

*The project's problem definition is*: 
> The challenge lies in accurately localizing a sound source on a two-dimensional plane
in real-time using audio pulses of a predefined frequency.
This involves addressing limitations in real-time processing, noise interference, and
the computational constraints of embedded systems while designing and implementing an effective DSP algorithm.

In more practical sense, it's a real-time processing system which locates the buzzer on a 2D plane. 

## Getting Started
**Clone by**: 

``git clone --recurse-submodules git@github.com:RichardKroesen/M33-Sound-Locator.git``

**Building Steps**: 
- mkdir build
- cmake ..
- make -j4

**Flashing steps**
Make sure the device is in BOOTSEL mode, this can be done by pressing the BOOTSEL button on the Pico. Afterwards there are two methods of uploading the file:

1. Using file explorer

Simply use a file explorer to drag the .uf2 file into the memory of the pico.

2. Using picotool

It is also possible to flash the device using picotool, type the command down below to flash using the picotool. This command assumes you are in the root of the project. NOT in the build folder!

`picotool load -u -v -x -t elf build/src/LINEAR_SYSTEMS.elf`

## Prerequisites 
For proper usage of this repository the following things are needed: 
- CMake
- PicoSDK (if it is not properly automatically build with CMake configuration).
- Arm GCC Compiler: [download page](https://developer.arm.com/downloads/-/gnu-rm)
- libusb [download page](https://libusb.info/)
- For debugging J-Link: [weblink](https://www.segger.com/downloads/jlink/) 
 
Or use on Linux the following command:
`sudo apt install cmake python3 g++ build-essential ninja-build`
installing the Arm GCC compiler (the hard way)
1) ```wget https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz```
2) ```tar -xvf arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz```
3) Move to a common location: ```sudo mv arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi /opt/```
4) Add to path: ```export PATH=$PATH:/opt/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi/bin```
5) For Pico SDK: ```export PICO_TOOLCHAIN_PATH=/opt/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi``` and ```export PATH=$PICO_TOOLCHAIN_PATH/bin:$PATH```

## Troubleshooting 
Since in our project group everyone is working on a different system there could be some compilation issues. In this section the most common issues are documented for quick reference. 

- CMake Error at build/_deps/pico_sdk-src/cmake/preload/toolchains/util/find_compiler.cmake:29 (message):
  Compiler 'arm-none-eabi-gcc' not found, you can specify search path with
  "PICO_TOOLCHAIN_PATH". ==> Repeating step 5 seems to fix this issue. 

After you cloned load the submodules properly:
``git submodule update --init`` 

## Project state
This repository contains the development and implementation of a real-time sound source localization system. The project integrates signal processing, embedded systems, and hardware design for accurate 2D localization.

Current version is contains an embedded codebase which is used as data logging acquisition. And then manually feeding these logs into model scripts so that system evaluation is feasible. The intent is to finalize this project someday as a hobby, but for now it is released for the course finalization. 

### Finalization Steps
- Implement TDOA algorithm on the embedded device => Python model conversion
- Integrate key signal processing features:
  - Target signal validation
  - Time Difference of Arrival (TDOA) estimation
  - Final system testing
- Produce a refined PCB design with:
  - Active bandpass filter
  - Variable amplifier
  - Stable reference voltage and improved grounding

### Lessons Learned
- Peak-based TDOA derivation is unreliable.
- Buzzers produce unsuitable waveforms for processing.
- Batch sampling introduces artifacts affecting analysis.
- Data transmission adds timing constraints but is irrelevant in a fully embedded system.

### Solutions Provided
- Designed PCB with anti-aliasing filter and amplifier.
- Developed real-time ADC DMA-based batch sampling (FreeRTOS).
- Implemented cross-correlation-based TDOA estimation.
- Achieved millimeter-precision real-time localization.
- Optimized target sound source detection using CMSIS-DSP.
- Provided insights for future improvements and finalization.

### Optimalization 
- Remove virtual interface implementation, which really not optimal and determinalistic approach. But it made prototyping convenient, the recommendation is to introduce curiously recurring template design pattern (CRTP), which makes polymorphism more determinalistic than currently. 
- For an even more enhancement dual-core functionally could be utilized for handling the ADC DMA on one core and the processing on the other. 
- ADC DMA logging and control is currently not efficient, since ASCII characters are send over instead of raw bytes. Besides the fact that the ADC also is blocked for around 20 ms (calculations are in our project report under appendix 1) before it can continue (we decided to keep it like this for simplicity and it worked well enough for our experimentation purpose). 

## Acknowledgements 
It is only fair to credit resources which inspired or gave an excellent example which already existed.

Our cmake FreeRTOS configuration is inspired by Dr. Jon Durrant, from [this repository](https://github.com/jondurrant/RPIPicoFreeRTOSSMPExp). 

Besides the Raspberry Pi Pico repository of FreeRTOS porting helped out, especially for RP2350 there were some troubles which are already described in the issues-list.