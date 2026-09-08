# TRT Simulator

This directory contains the first executable TRT-compatible board implementation.

The simulator validates protocol, dispatcher, state, configuration identity values, response model, and error model before any hardware target.

## Implemented Command Scope

Supported commands:

1. PING
2. FW_VERSION
3. BOARD_VERSION
4. INFO
5. STATUS
6. CAPABILITIES

All other commands:

1. NACK
2. ERROR_UNSUPPORTED_COMMAND (for known but unsupported command IDs)
3. ERROR_UNKNOWN_COMMAND (for unknown command IDs)

## Simulator Identity

1. Board Type: SIMULATOR
2. Board Revision: A1
3. Firmware Version: 0.1.0
4. Protocol Version: 0.1.0 (wire value 0x01)
5. Serial Number: SIM000001

## Build

From repository root:

```powershell
cmake -S . -B build
cmake --build build
```

## Run

Interactive raw-frame mode (stdin -> stdout, hex bytes):

```powershell
build\trt_simulator_app.exe
```

Provide one hex frame per line. The simulator returns one hex response frame per line.

## Logging Hooks

The simulator emits runtime debug logs to stderr for:

1. RX Frame
2. TX Frame
3. Decoded Command
4. Response
5. State
