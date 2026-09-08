# TRT Simulator Architecture

## Purpose

The simulator is the first official TRT device implementation target for protocol validation phase.

It validates command/response behavior before any hardware board implementation begins.

## Scope for V0.1

Simulator supports only:

1. PING
2. FW_VERSION
3. BOARD_VERSION
4. INFO
5. STATUS
6. CAPABILITIES

All other commands:

1. Return NACK
2. Return ERROR_UNSUPPORTED_COMMAND

## Required Runtime Behavior

1. Execute same frame parsing rules as real boards.
2. Execute same command identifiers and response identifiers.
3. Execute same error model semantics.
4. Preserve non-blocking loop behavior.

TRT-cli should not detect whether target is simulator or hardware through protocol behavior.

## Data Sources

Simulator response values should be sourced from configuration and runtime state:

1. Board Type: SIMULATOR
2. Board Revision: A1 (or configured value)
3. Firmware Version: 0.1.0 baseline
4. Protocol Version: 0x01

## Example Interactions

Example request:

```text
trt board board0 ping
```

Example response intent:

```text
ACK
```

Example request:

```text
trt board board0 info
```

Example response intent:

```text
Board Type: SIMULATOR
Board Revision: A1
Firmware Version: 0.1.0
Protocol Version: 1
```

## Why Simulator Comes First

1. Removes hardware variability during protocol hardening.
2. Enables deterministic automated tests.
3. Shortens feedback loop between TRT-cli and TRT-core protocol logic.
4. Reduces integration risk before Arduino/ESP32/STM32/RP2040 bring-up.

## First Implementation Order

1. Simulator
2. Arduino
3. ESP32
4. STM32

RP2040 follows after protocol parity on first hardware set.
