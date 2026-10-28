"""Send one TRT v0.1 PING to the STM32 ST-LINK virtual COM port."""

from __future__ import annotations

import argparse
import struct
import sys

import serial

SYNC = b"\xaa\x55"
BOARD_ID = 0x0002
PING = 0x0001
ACK = 0x8001
HEADER_SIZE = 12


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", default="COM4", help="ST-LINK VCP port (default: COM4)")
    args = parser.parse_args()

    sequence_id = 1
    header = SYNC + bytes((1, 0)) + struct.pack(">HHHH", BOARD_ID, sequence_id, PING, 0)
    request = header + b"\x00\x00"

    try:
        with serial.Serial(
            args.port,
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=2,
            write_timeout=2,
        ) as connection:
            connection.reset_input_buffer()
            connection.write(request)
            connection.flush()

            response_header = connection.read(HEADER_SIZE)
            if len(response_header) != HEADER_SIZE:
                raise TimeoutError(
                    f"Timed out waiting for the 12-byte response header on {args.port}"
                )
            if response_header[:2] != SYNC or response_header[2] != 1:
                raise ValueError(f"Invalid TRT response header: {response_header.hex(' ')}")

            board_id, response_sequence, command, payload_length = struct.unpack(
                ">HHHH", response_header[4:12]
            )
            if payload_length != 0:
                raise ValueError(f"PING ACK must have an empty payload, got {payload_length} bytes")
            payload_and_crc = connection.read(payload_length + 2)
            if len(payload_and_crc) != payload_length + 2:
                raise TimeoutError(f"Incomplete TRT response from {args.port}")
    except (OSError, serial.SerialException, TimeoutError, ValueError) as error:
        print(f"PING failed on {args.port}: {error}", file=sys.stderr)
        return 1

    if (
        board_id != BOARD_ID
        or response_sequence != sequence_id
        or command != ACK
        or payload_and_crc[-2:] != b"\x00\x00"
    ):
        print(
            "Unexpected response: "
            f"board=0x{board_id:04X}, seq=0x{response_sequence:04X}, "
            f"command=0x{command:04X}, payload={payload_and_crc[:-2].hex(' ')}",
            file=sys.stderr,
        )
        return 1

    print(f"PING OK: {args.port} returned ACK 0x{command:04X} for board 0x{board_id:04X}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
