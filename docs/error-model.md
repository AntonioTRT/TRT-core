# TRT Protocol V0.1 Error Model

## Purpose

This document freezes runtime error identifiers for protocol-level interoperability and deterministic host behavior.

## Error Identifier Table

| ID (hex) | Name | Meaning |
|---|---|---|
| 0x0001 | ERROR_UNKNOWN_COMMAND | Command ID is not recognized by this protocol implementation |
| 0x0002 | ERROR_UNSUPPORTED_COMMAND | Command ID is known but not supported by active runtime/capability policy |
| 0x0003 | ERROR_INVALID_ARGUMENT | Payload format, length, or value constraints are invalid |
| 0x0004 | ERROR_BUSY | Runtime is currently in a busy state and cannot execute request now |
| 0x0005 | ERROR_TIMEOUT | Operation exceeded allowed processing window |
| 0x0006 | ERROR_INTERNAL | Internal runtime fault prevented completion |

## Error Transport Rule

Errors are returned through:

1. Response ID: NACK (0x8002)
2. NACK payload: error identifier from this table

## Mandatory Unsupported Behavior

For any command outside mandatory V0.1 support:

1. Return NACK
2. Return ERROR_UNSUPPORTED_COMMAND

This rule is mandatory for TRT compatibility.

## Error Usage Guidance

1. Use ERROR_UNKNOWN_COMMAND for truly unknown IDs.
2. Use ERROR_UNSUPPORTED_COMMAND when command is recognized but intentionally unavailable.
3. Use ERROR_INVALID_ARGUMENT when frame payload semantics are wrong.
4. Use ERROR_BUSY when runtime policy defers execution.
5. Use ERROR_TIMEOUT for bounded asynchronous or staged operations.
6. Use ERROR_INTERNAL for unexpected faults after validation passes.

## Why This Model Exists

1. Enables deterministic CLI messaging and retry strategy.
2. Keeps board-family differences out of host error handling paths.
3. Supports consistent simulator-to-hardware migration.
