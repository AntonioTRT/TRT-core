# TRT Protocol V0.1 Wire Format

## Status

This document freezes the TRT Protocol V0.1 wire format as the official communication standard for TRT-core and TRT-cli interoperability.

Version scope:

- Protocol version: 0.1.0
- Wire VERSION field value: 0x01

## Why This Is Frozen

Protocol validation must complete before hardware feature development so that:

1. CLI and board runtimes share one deterministic frame structure.
2. Simulator and real boards behave identically on the wire.
3. Firmware teams can implement transport adapters without protocol ambiguity.

## Frame Structure

Frame fields and sizes:

1. SYNC: 2 bytes
2. VERSION: 1 byte
3. FLAGS: 1 byte
4. BOARD_ID: 2 bytes
5. SEQ_ID: 2 bytes
6. COMMAND: 2 bytes
7. LENGTH: 2 bytes
8. PAYLOAD: variable
9. CRC16: 2 bytes

Canonical layout:

```text
SYNC | VERSION | FLAGS | BOARD_ID | SEQ_ID | COMMAND | LENGTH | PAYLOAD | CRC16
```

Base frame size without payload:

$$2 + 1 + 1 + 2 + 2 + 2 + 2 + 2 = 14\ bytes$$

## Field Definitions

### SYNC

- Size: 2 bytes
- Value: 0xAA55
- Purpose: packet synchronization marker

Rationale:

- A fixed sync word reduces false-positive frame starts and simplifies parser recovery from stream misalignment.

### VERSION

- Size: 1 byte
- Initial value: 0x01
- Purpose: protocol version indicator used by parser and compatibility checks

Rationale:

- A dedicated version byte enables future revisions while preserving backward-compatibility logic.

### FLAGS

- Size: 1 byte
- Initial value: 0x00
- Purpose: reserved extension space

Planned future flag classes:

1. Response
2. Event
3. Broadcast
4. Debug

Rationale:

- Reserving flags now avoids frame redesign later.

### BOARD_ID

- Size: 2 bytes
- Type: uint16
- Range: 0x0001 to 0xFFFF

Rationale:

- Uniform board addressing supports single-device and multi-device transport topologies.

### SEQ_ID

- Size: 2 bytes
- Type: uint16
- Purpose: request/response matching, chunked transfers, and future firmware-update sequencing

Rationale:

- Sequence correlation makes retry/reorder handling explicit and transport-independent.

### COMMAND

- Size: 2 bytes
- Type: uint16
- Purpose: references the V0.1 command table

Rationale:

- Numeric command identifiers eliminate ambiguity and ensure binary interoperability.

### LENGTH

- Size: 2 bytes
- Type: uint16
- Purpose: payload length in bytes

Validation rule:

- LENGTH must exactly match PAYLOAD byte count.

### PAYLOAD

- Size: variable
- Purpose: command arguments or response data

### CRC16

- Size: 2 bytes
- Purpose: frame integrity validation

V0.1 policy:

- CRC16 is a required frame field.
- CRC algorithm details are deferred to a follow-up protocol patch.
- Parser and encoder interfaces must preserve field placement and size.

## Parsing and Encoding Rules

1. Parser validates field boundaries and LENGTH consistency.
2. Parser validates SYNC and VERSION before command dispatch.
3. Frames failing validation are rejected with protocol error handling.
4. Encoder must emit fields in canonical order without transport-specific variations.

## Expansion Strategy

Future protocol versions may add semantics, but V0.1 frame layout remains backward reference baseline for all compliance tests.
