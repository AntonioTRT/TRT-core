# Firmware V0.1 Roadmap

## Purpose

This roadmap defines the first implementation path for TRT Protocol V0.1 firmware bring-up on real hardware and simulator targets.

Scope is intentionally minimal:

1. PING
2. BOARD_VERSION
3. FW_VERSION
4. INFO
5. STATUS
6. CAPABILITIES
7. ERROR_UNSUPPORTED_COMMAND for all other commands

No peripheral-control functionality is part of this milestone.

## Definition of Done for V0.1

A firmware target is considered V0.1-ready when it can:

1. Boot and initialize TRT runtime.
2. Process protocol frames without blocking board operation.
3. Respond correctly to all mandatory commands.
4. Return ERROR_UNSUPPORTED_COMMAND for unknown/non-V0.1 commands.
5. Keep internal state/services running while command traffic is active.

## Minimum Viable Firmware Profile

Target examples:

1. Arduino Uno or Nano
2. ESP32 DevKit
3. STM32 Nucleo

Minimum runtime responsibilities:

1. Boot sequence and configuration load.
2. State machine initialization (BOOT -> IDLE/READY).
3. Communication initialization through one ITransport adapter.
4. Parser -> RX Queue -> Command Queue -> Dispatcher -> TX Queue flow.
5. Mandatory V0.1 command responses.

## Phase 1: Core Bring-Up

### Goals

1. Boot runtime and construct BoardContext.
2. Initialize Configuration Manager from compile-time defaults, then persistent overrides if available.
3. Initialize state machine and communication loop.
4. Implement command routing for mandatory V0.1 commands.

### Required Outputs

1. Board boots into deterministic state.
2. Host can connect and receive valid responses for all mandatory commands.
3. Unknown commands return ERROR_UNSUPPORTED_COMMAND.

### Exit Criteria

1. TRT-cli discovery can identify board.
2. Status and capabilities responses are consistent.
3. Runtime remains responsive under repeated command traffic.

## Phase 2: Simulator Validation

### Goals

1. Validate parser/dispatcher behavior in deterministic environment.
2. Validate state transitions and status reporting.
3. Validate error behavior for unknown commands.

### Exit Criteria

1. Automated test scenarios pass for mandatory command set.
2. Queue flow and response timing remain non-blocking.

## Phase 3: Arduino Validation

### Goals

1. Port minimal runtime to Arduino target.
2. Validate serial transport path with TRT-cli.
3. Confirm mandatory command compliance on real device.

### Exit Criteria

1. Flash and boot reliability across power cycles.
2. Stable command/response behavior over serial link.
3. Correct ERROR_UNSUPPORTED_COMMAND behavior for non-mandatory commands.

## Phase 4: STM32 Validation

### Goals

1. Port same V0.1 runtime profile to STM32 target.
2. Reuse shared core logic with platform-specific adapters.
3. Confirm compatibility parity with simulator and Arduino behavior.

### Exit Criteria

1. Mandatory command response parity achieved.
2. No architecture forks introduced in TRT-core.

## Phase 5: ESP32 Validation

### Goals

1. Port same V0.1 runtime profile to ESP32 target.
2. Validate transport and runtime behavior under board-specific timing.
3. Confirm autonomous execution while serving command traffic.

### Exit Criteria

1. Full mandatory command compliance.
2. Consistent status/capability reporting with other targets.
3. Stable operation with host connect/disconnect cycles.

## Cross-Phase Quality Gates

1. No direct hardware-control commands added.
2. No transport-specific protocol branching added to TRT-core parser/dispatcher layers.
3. No blocking wait-for-message loop patterns introduced.
4. Mandatory command set remains the only accepted command scope for V0.1.
