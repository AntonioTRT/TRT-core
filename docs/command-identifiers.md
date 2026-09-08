# TRT Protocol V0.1 Command Identifiers

## Status

This document freezes the first official TRT command identifier table for protocol V0.1.

These identifiers are mandatory and platform-independent.

## Mandatory Command Table

| ID (hex) | Name | Direction | Required |
|---|---|---|---|
| 0x0001 | PING | Host -> Board | Yes |
| 0x0002 | FW_VERSION | Host -> Board | Yes |
| 0x0003 | BOARD_VERSION | Host -> Board | Yes |
| 0x0004 | INFO | Host -> Board | Yes |
| 0x0005 | STATUS | Host -> Board | Yes |
| 0x0006 | CAPABILITIES | Host -> Board | Yes |

Compatibility rule:

- Every TRT-compatible board MUST implement all identifiers above.

## Mandatory Runtime Behavior

1. Known mandatory command IDs are always recognized.
2. Non-mandatory or unknown command IDs return NACK with ERROR_UNSUPPORTED_COMMAND.
3. Command execution must remain non-blocking and queue-based.

## Why IDs Are Frozen Now

1. CLI and simulator need deterministic binary interoperability.
2. Hardware bring-up must not block on command naming debates.
3. Trace and test tooling requires stable numeric keys.

## Expansion Strategy

Future command families may append new IDs above V0.1 range while preserving backward compatibility for 0x0001 to 0x0006.
