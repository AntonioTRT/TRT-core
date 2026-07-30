# Protocol Integration

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

- No command IDs are defined in this repository.
- No command business logic is implemented in this repository.
- No transport-specific protocol behavior is hardcoded into parser/encoder contracts.
