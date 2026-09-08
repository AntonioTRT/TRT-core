# TRT Protocol V0.1 Response Model

## Purpose

This document defines the response envelope semantics shared by simulator and future hardware targets.

## Response Identifier Table

| ID (hex) | Name | Purpose |
|---|---|---|
| 0x8001 | ACK | Command accepted and completed without payload data requirement |
| 0x8002 | NACK | Command rejected or failed; payload carries error identifier |
| 0x8003 | DATA | Command completed with payload data |

## Response Selection Rules

1. Use ACK for successful no-data outcomes.
2. Use DATA for successful outcomes that return payload fields.
3. Use NACK for invalid, unsupported, timed-out, or internal-failure conditions.

## Mandatory V0.1 Command Response Patterns

1. PING -> ACK (or DATA if implementation includes explicit token echo)
2. FW_VERSION -> DATA
3. BOARD_VERSION -> DATA
4. INFO -> DATA
5. STATUS -> DATA
6. CAPABILITIES -> DATA

Unknown command path:

1. Unknown or unsupported command ID -> NACK
2. NACK payload includes ERROR_UNSUPPORTED_COMMAND identifier

## Why Separate Response IDs

1. Keeps success/failure semantics explicit on the wire.
2. Simplifies CLI decode and diagnostics.
3. Prevents ambiguity between empty success and empty failure responses.

## Interoperability Contract

TRT-cli must decode responses purely by protocol identifiers and payload schema, not by board-family assumptions.
