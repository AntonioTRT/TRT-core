# Board Porting Guide

This document describes how to port TRT-core to a new board family while preserving shared runtime architecture.

## Porting Goal

Replace board-specific implementations only, while reusing 80-90% of TRT-core logic.

## Required Steps

1. Implement required hardware interfaces (`IGpio`, `IPwm`, `IAdc`, `IDac`, `II2c`, `ISpi`, and others as needed).
2. Implement one or more `ITransport` adapters.
3. Build board metadata (`BoardInfo`).
4. Register supported capabilities in `CapabilityManager`.
5. Register installed modules in `ModuleManager`.
6. Compose and expose everything through `BoardContext`.

## Core Rules

1. Do not add board-specific conditionals in TRT-core dispatcher/parser/encoder layers.
2. Do not couple command routing to concrete board classes.
3. Keep board-specific logic inside `boards/<family>/` and related drivers outside core contracts.

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
- Command integration testing without physical hardware
