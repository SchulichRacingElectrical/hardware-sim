# Schulich Racing Telemetry Simulator (SRTS)

This project was developed to test SR Velocity without a hardware bench,
and stress test our system as a whole.

## Definitions

- **VFDCP**: Variable Frequency Data Compression Protocol
- **FFDT**: Frequency Forward Data Transmission
- **Thing**: An object (i.e. car or toaster) that has a set of sensors and hardware that are compatible with this system.
- **Sensor**: A sensing instrument on a "Thing" (e.g., potentiometer)
- **Channel**: A medium through which sensors send their data.
- **Stream**: An aggregation of channels that forms a single point of data.

## Methodology

The idea behind this system is allow the Schulich Racing team to test
upcoming software without hardware. This system, is effectively an
emulation of our vehicle's onboard hardware. The system can spawn multiple
"Things" asynchronously to send to our servers. Our system as a whole
should be able to handle multiple objects streaming data for real-time
visualization and data storage.

VFDCP was developed with the idea that we should not send all of our
sensor data at a fixed rate, nor should we send data that has not changed.
Additionally, the protocol should support real-time data and minimize latency.
VFDCP encodes data into a byte array following format:

`Sensor Count (1 Byte) + Timestamp (4 Bytes) + Sensor IDs (Variable; Number of sensors) + Sensor Data (Variable; Number of sensors and the byte width of sensors values)`

For Schulich Racing, FSAE's vehicles, most of our sensors run at low
frequencies. The range is `1-120 Hz`, but the average rate is `~10 Hz`. Using
VFDCP, the team realizes up to 80% lower data transmission costs. However,
it suffers from potential loss by using UDP. MQTT was avoided due to
higher latency, and TCP acks; while MQTT has a smaller header size, we
felt this solution would do us better.

FFDT was developed to solve the problem of misaligned frequencies. For example, if you have sensors with frequencies of `1, 3, 5, 10 Hz`, there will
be situations where we need to send data at a non-linear time scale. Since
this increases transmission costs, we want to send data at the highest sensor
frequency. We carry forward data that should have been sent on a more recent tick to the next frequency alignment of `10 Hz`.

## Running

Ensure that you have CMake installed and can use `C++20`. This project was
developed with `clang++` on an `ARM64 Apple Silicon` machine with VSCode. If you have the same hardware, you will need CMake, CMake Tools, and CodeLLDB to build and debug the program.

This program also uses Google Test, which should be downloaded and put into the `lib` directory in the root folder.

With the prerequisites, you may build the project using F7, or debug with F5. When building the program, there will be a `build` folder in the root directory. The executable is in `build/source/TelemetrySim_run`. The test executable is in `build/test/TelemetrySim_test`.

You may use the command line interface to see how the project works. You have the option to create telemetry "Things", view them, and start telemetry session. At this time, the solution is fairly barebones as tasks were prioritized to develop the core technology.

## Future Work

This project is in pre-pre-alpha. There are various design and implementation issues that need to be revisited. Additionally, the system must be tested more rigourously.

Core features to add are integrating database, REST, and telemetry endpoints.
Primarily, networking features need to be added. Our car stores sensors in flash and this system should also store data locally where possible. We have a
"sensor diff" algorithm that will allow us to only pull sensors that have been
updated from our database. In general, we should integrate the program with
SR Velocity's backend systems and complete the hardware simulation.

Future features include the development of a robust interface using Qt.

## Design

Please find a lengthy, hand-written design document used to build this system
in the design folder. This document has not been updated since the initial design.

# Licenses

Special thanks to:

GoogleTest - Copyright 2008, Google Inc. - BSD-3-Clause\
JSON for Modern C++ - Copyright (c) 2013-2022 Niels Lohmann - MIT License\
httplib - Copyright (c) 2021 Yuji Hirose. All rights reserved. - MIT License\

This software is available through the MIT License provided in the repository.

Copyright 2022 © Schulich Racing, FSAE
