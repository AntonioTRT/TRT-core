# TRT Protocol V0.1 Command Set

## Status

- Protocol milestone: 0.1.0
- Purpose: establish the first mandatory interoperability baseline for all TRT-compatible boards.
- Scope: discovery, identity, status, and capability reporting only.

This document defines command semantics bound to frozen V0.1 command identifiers from docs/command-identifiers.md.
Transport-specific behavior and hardware driver behavior are out of scope.

## Compatibility Requirement

A board is TRT-compatible for protocol milestone 0.1.0 only if it supports all commands in this mandatory set:

1. 0x0001 PING
2. 0x0002 FW_VERSION
3. 0x0003 BOARD_VERSION
4. 0x0004 INFO
5. 0x0005 STATUS
6. 0x0006 CAPABILITIES

If any mandatory command is missing, the board is not TRT-compatible for V0.1.

## Architectural Intent

This command set validates:

1. Protocol parser/encoder integration
2. Transport abstraction correctness
3. Discovery and board identification
4. Runtime status reporting
5. Capability-driven behavior advertisement

No peripheral-control commands are part of V0.1.

## Shared Command Contract Rules

1. Command IDs are fixed in V0.1 and must match docs/command-identifiers.md.
2. Request/response handling must be non-blocking and queue-based.
3. Commands must not directly manipulate hardware outputs.
4. Responses should be generated from runtime state and configuration sources.
5. Unsupported optional features still require command-level response consistency.

## Command Definitions

### PING

Purpose:

- Connectivity and runtime liveness check.

Request:

- No required payload fields.

Response:

- Acknowledge board liveness.
- Include sequence correlation where protocol framing supports it.

Behavior Notes:

- Must complete quickly and never block service/state execution.
- Does not change board state.

### BOARD_VERSION

Purpose:

- Return board hardware identity and revision metadata.

Request:

- No required payload fields.

Response:

- Board family/type identifier.
- Board revision.
- Optional hardware profile tags.

Payload format:

1. ASCII string for board revision.

Example:

```text
A1
```

Behavior Notes:

- Values should come from board metadata and configuration source-of-truth.
- Must not require host polling loops or side effects.

### FW_VERSION

Purpose:

- Return firmware/runtime version metadata.

Request:

- No required payload fields.

Response:

- TRT-core integration version.
- Board firmware version string/tuple.
- Protocol version compatibility marker.

Payload format (fixed 3 bytes):

1. BYTE0 = Major
2. BYTE1 = Minor
3. BYTE2 = Patch

Example bytes:

```text
00 01 00
```

Interpreted as:

- 0.1.0

Behavior Notes:

- Intended for host compatibility checks during discovery.

### INFO

Purpose:

- Return consolidated static identity information used by host discovery.

Request:

- No required payload fields.

Response:

- Board name.
- Board type.
- Revision.
- Serial number (or deterministic placeholder policy).
- Protocol version marker.

Recommended payload structure:

1. FIELD_COUNT (1 byte)
2. Repeated tuples: FIELD_ID (1 byte), FIELD_LENGTH (1 byte), FIELD_VALUE (N bytes)

Recommended field IDs:

1. 0x01 = Board Type (ASCII)
2. 0x02 = Board Revision (ASCII)
3. 0x03 = Firmware Version (3 bytes major/minor/patch)
4. 0x04 = Protocol Version (1 byte)
5. 0x05 = Serial Number (ASCII)

Rationale:

- Tuple format keeps parser behavior deterministic while allowing backward-compatible field growth.

Behavior Notes:

- Info response should be stable across session unless configuration changes.

### STATUS

Purpose:

- Return live runtime status snapshot.

Request:

- No required payload fields.

Response:

- Current state-machine state.
- Health/severity summary.
- Operational mode summary.
- Optional queue-depth telemetry fields.

Minimum payload format (fixed 1 byte):

1. BYTE0 = state code

State code mapping:

1. 0x00 = BOOT
2. 0x01 = READY
3. 0x02 = BUSY
4. 0x03 = WARNING
5. 0x04 = ERROR
6. 0x05 = FIRMWARE_UPDATE

Optional extension fields may be appended in future minor versions for health and queue telemetry.

Behavior Notes:

- Must report runtime facts without pausing runtime behavior.
- Supports host observability while preserving board autonomy.

### CAPABILITIES

Purpose:

- Return declared board capabilities for generic command gating.

Request:

- No required payload fields.

Response:

- Capability list (for example GPIO, PWM, ADC, DAC, SPI, I2C, LCD, LED, RELAY).

Recommended payload format:

1. COUNT (1 byte)
2. Repeated capability identifiers (uint16 each)

Rationale:

- Count + fixed-width identifiers provides deterministic cross-platform parsing.

Behavior Notes:

- Capability list is board-declared and runtime-owned.
- Host should adapt behavior from this response instead of hardcoding board assumptions.

## Error and Compatibility Behavior

1. Mandatory commands must always be recognized.
2. Optional or future commands may return ERROR_UNSUPPORTED_COMMAND.
3. CAPABILITIES response is authoritative for feature discovery.
4. Command execution failures should preserve runtime continuity.
5. Unsupported command behavior is mandatory: return NACK with ERROR_UNSUPPORTED_COMMAND.

## Queue Integration Expectations

Mandatory command handling path:

1. Transport receive
2. Parser decode
3. RX Queue
4. Command Queue
5. Dispatcher
6. State-aware service execution
7. TX Queue response
8. Transport send

The board must continue autonomous service/state updates while this path executes.

## Out of Scope for V0.1

1. GPIO/PWM/ADC/DAC/SPI/I2C direct control commands
2. Module-specific control commands
3. Transport-specific protocol variants
4. Hardware driver implementation details

