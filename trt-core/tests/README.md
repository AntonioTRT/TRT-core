# TRT Simulator Validation Tests

This directory provides executable tests for TRT Protocol V0.1 simulator behavior.

## Covered Cases

1. PING -> PONG
2. FW_VERSION -> 0.1.0 bytes
3. BOARD_VERSION -> A1
4. INFO -> simulator identity block
5. STATUS -> READY
6. CAPABILITIES -> SYSTEM
7. Unknown command -> ERROR_UNKNOWN_COMMAND
8. Unsupported command -> ERROR_UNSUPPORTED_COMMAND

## Build

```powershell
cmake -S . -B build
cmake --build build
```

## Run

```powershell
ctest --test-dir build --output-on-failure
```

Expected result:

- `trt_protocol_simulator_tests` passes.
