# Simulator Examples

This directory hosts executable simulator usage examples for TRT Protocol V0.1.

Primary example:

1. `protocol_walkthrough.cpp`

It demonstrates command/response flow for:

1. PING
2. FW_VERSION
3. BOARD_VERSION
4. INFO
5. STATUS
6. CAPABILITIES

## Build

```powershell
cmake -S . -B build
cmake --build build
```

## Run

```powershell
build\trt_simulator_example.exe
```

Expected output includes:

1. `PING -> PONG`
2. `FW_VERSION -> 00 01 00`
3. `BOARD_VERSION -> A1`
4. `STATUS -> 1`
5. `CAPABILITIES -> 01 00 01`


