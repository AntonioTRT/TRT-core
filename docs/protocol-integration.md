# Protocol Integration

## Protocol Milestone

- Current official milestone: TRT Protocol V0.1.0
- Top-level protocol specification: docs/protocol.md

This milestone validates architecture, transport abstraction, identity/discovery, status reporting, and capability reporting before any peripheral-control command families are introduced.

TRT protocol frame format is already defined externally and includes:

- `SYNC`
- `VERSION`
- `FLAGS`
- `BOARD_ID`
- `SEQ_ID`
- `COMMAND`
- `LENGTH`
- `PAYLOAD`
- `CRC16`

TRT-core does not redefine command numbers or protocol ownership. It integrates with this protocol through parser/encoder contracts.

In V0.1, command IDs are frozen and documented in docs/command-identifiers.md.

## Integration Responsibilities

`ProtocolParser` responsibilities:

- Accept raw bytes from transport
- Validate minimum frame shape and integrity hooks
- Produce structured frame object for dispatcher

`ProtocolEncoder` responsibilities:

- Convert response frame object to raw bytes
- Prepare bytes for transport send

## Dispatcher Integration Flow

1. Receive raw bytes from transport.
2. Parse raw bytes into protocol frame.
3. Route frame by command to command dispatcher.
4. Perform capability checks during handling.
5. Invoke board logic through interfaces only.
6. Build response frame.
7. Encode response frame.
8. Send response bytes through transport.

## Constraints

- Command identifiers, response identifiers, and error identifiers are frozen for V0.1 documentation phase.
- No command business logic is implemented in this repository.
- No transport-specific protocol behavior is hardcoded into parser/encoder contracts.
- Mandatory command semantics for V0.1 are documented in docs/commands-v0.1.md.


