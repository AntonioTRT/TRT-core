# Board Porting Guide

This document describes how to port TRT-core to a new board family while preserving shared runtime architecture.

## Porting Goal

Replace board-specific implementations only, while reusing 80-90% of TRT-core logic.

Board firmware must remain autonomous and keep local behavior active without host connection.

Board compatibility also requires implementing the mandatory TRT Protocol V0.1 command set documented in docs/commands-v0.1.md.

## Required Steps

1. Implement required hardware interfaces (`IGpio`, `IPwm`, `IAdc`, `IDac`, `II2c`, `ISpi`, and others as needed).
2. Implement one or more `ITransport` adapters.
3. Build board metadata (`BoardInfo`).
4. Register supported capabilities in `CapabilityManager`.
5. Register installed modules in `ModuleManager`.
6. Provide configuration defaults and persistent configuration binding.
7. Compose and expose everything through `BoardContext`.
8. Execute non-blocking service/event/command loop.
9. Implement and validate mandatory V0.1 commands (no command IDs required in this phase).

## Core Rules

1. Do not add board-specific conditionals in TRT-core dispatcher/parser/encoder layers.
2. Do not couple command routing to concrete board classes.
3. Keep board-specific logic inside `boards/<family>/` and related drivers outside core contracts.
4. Do not block runtime waiting for host messages.
5. Do not model the board as a remote GPIO expander.

## Family Placeholders

Current placeholder families:

- STM32
- ESP32
- Arduino
- RP2040
- Simulator

Raspberry Pi class support can follow the same model by implementing interfaces and transport adapters with Linux-compatible drivers.

## Simulator Port

Simulator is treated as another board family.

Expected simulator contracts:

- Mock `IGpio`
- Mock `IPwm`
- Mock `IAdc`
- At least one loopback/in-memory transport

Purpose:

- Protocol parser and encoder validation
- Dispatcher and capability behavior testing
- State-machine and service behavior testing without physical hardware
- CLI development against a board-like endpoint

## New Board Family Checklist

1. Implement interfaces and transport adapters.
2. Register capabilities and modules.
3. Integrate configuration defaults and persistent load.
4. Map service update cadence to platform timing primitives.
5. Validate command/event queue behavior under load.
6. Validate autonomous behavior with host disconnected.
7. Validate mandatory V0.1 command responses for discovery/status/capabilities.


