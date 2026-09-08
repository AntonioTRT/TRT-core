# TRT Protocol V0.1 Specification

## Purpose

This document is the top-level protocol specification for TRT Protocol V0.1.

It defines the official and stable communication contract between TRT-cli and TRT-compatible devices.

## Protocol Scope

V0.1 focuses on foundational interoperability:

1. Frame-level communication
2. Board discovery
3. Board identity
4. Runtime status
5. Capability reporting
6. Deterministic response and error semantics

No peripheral control commands are part of V0.1.

## Normative References

1. Wire format: docs/protocol-v0.1-wire-format.md
2. Command IDs: docs/command-identifiers.md
3. Response IDs: docs/response-model.md
4. Error IDs: docs/error-model.md
5. Mandatory command semantics: docs/commands-v0.1.md
6. Simulator behavior: docs/simulator.md
7. Readiness gates: docs/protocol-readiness-checklist.md

## Compatibility Rules

1. Every TRT-compatible board implements mandatory V0.1 commands.
2. Every board uses the frozen V0.1 frame layout.
3. Unsupported commands return NACK + ERROR_UNSUPPORTED_COMMAND.
4. CLI behavior remains board-agnostic and protocol-driven.

## Mandatory Command Set

1. 0x0001 PING
2. 0x0002 FW_VERSION
3. 0x0003 BOARD_VERSION
4. 0x0004 INFO
5. 0x0005 STATUS
6. 0x0006 CAPABILITIES

## First Device Validation Path

1. Simulator validates protocol conformance.
2. Arduino validates first hardware transport integration.
3. ESP32 and STM32 validate cross-family parity.

This sequence minimizes protocol risk before full platform expansion.

## Implementation-Phase Artifacts

Executable simulator and validation assets:

1. simulator/ (runtime implementation)
2. examples/simulator/ (protocol walkthrough executable)
3. tests/ (protocol conformance tests for simulator)
