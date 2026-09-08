#!/usr/bin/env python3

from __future__ import annotations

import csv
import json
import re
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

# ============================================================
# USER CONFIGURATION
# ============================================================

COM_PORT = "COM4"

PROJECT = "ledringandencoder"

BOARD = "uno"

BOARD_ID = 101

FW_VERSION = "0.1.0"

CAPABILITIES = ["SYSTEM"]

SERIAL_BAUD = 115200

REDIRECT_APPLICATION_SERIAL_TO_SERIAL1 = False

TRT_HAS_STL = 0
TRT_HAS_DYNAMIC_MEMORY = 0
TRT_HAS_MODULES = 0
TRT_MAX_FRAME_SIZE = 64

# ============================================================
# GENERATED VALUES
# ============================================================

BOARD_NAME = PROJECT
BOARD_TYPE = BOARD.upper()
BOARD_REVISION = "UNSPECIFIED"

# ============================================================
# PATHS
# ============================================================

ROOT = Path(__file__).resolve().parents[1]

SKETCH = ROOT / "TRT-arduino" / PROJECT / f"{PROJECT}.ino"

BUILD_ROOT = ROOT / ".trt-build" / PROJECT

REGISTRY = ROOT / "build_registry.csv"

PLATFORMIO_WRAPPER = BUILD_ROOT / "platformio_wrapper.py"

# ============================================================
# BUILD ID MANAGEMENT
# ============================================================

def next_build_id() -> int:
    if not REGISTRY.exists():
        return 1

    with REGISTRY.open(
        newline="",
        encoding="utf-8"
    ) as registry_file:

        rows = list(csv.DictReader(registry_file))

    return max(
        (int(row["build_id"]) for row in rows),
        default=0
    ) + 1


# ============================================================
# GIT
# ============================================================

def git_commit() -> str:
    try:
        return subprocess.check_output(
            [
                "git",
                "-C",
                str(ROOT),
                "rev-parse",
                "--short",
                "HEAD",
            ],
            text=True,
            stderr=subprocess.DEVNULL,
        ).strip()

    except Exception:
        return ""


# ============================================================
# METADATA
# ============================================================

def write_metadata(build_id: int) -> None:

    metadata = {
        "build_id": f"{build_id:06d}",
        "board_id": BOARD_ID,
        "board_name": BOARD_NAME,
        "board_type": BOARD_TYPE,
        "board_revision": BOARD_REVISION,
        "firmware_version": FW_VERSION,
        "capabilities": CAPABILITIES,
    }

    (BUILD_ROOT / "trt-board.json").write_text(
        json.dumps(metadata, indent=2),
        encoding="utf-8",
    )

    profile = f"""
#pragma once

#include <stdint.h>

namespace trt_profile {{

constexpr uint16_t kBoardId = {BOARD_ID};
constexpr uint32_t kBuildId = {build_id};
constexpr uint8_t kFirmwareVersionMajor = {FW_VERSION.split('.')[0]};
constexpr uint8_t kFirmwareVersionMinor = {FW_VERSION.split('.')[1]};
constexpr uint8_t kFirmwareVersionPatch = {FW_VERSION.split('.')[2]};

constexpr unsigned long kSerialBaud =
    {SERIAL_BAUD}UL;

constexpr char kBoardName[] =
    "{BOARD_NAME}";

constexpr char kBoardType[] =
    "{BOARD_TYPE}";

constexpr char kBoardRevision[] =
    "{BOARD_REVISION}";

constexpr char kFirmwareVersion[] =
    "{FW_VERSION}";

}}
"""

    (BUILD_ROOT / "src" / "trt_board_profile.h").write_text(
        profile,
        encoding="utf-8",
    )


# ============================================================
# STAGE SOURCES
# ============================================================

def write_platformio_wrapper() -> None:
    wrapper = """import truststore

truststore.inject_into_ssl()

from platformio.__main__ import main

raise SystemExit(main())
"""
    PLATFORMIO_WRAPPER.write_text(
        wrapper,
        encoding="utf-8",
    )


def run_platformio(*arguments: str) -> None:
    command = [
        sys.executable,
        str(PLATFORMIO_WRAPPER),
        *arguments,
    ]
    print("\nExecuting:")
    print(" ".join(command))

    completed = subprocess.run(
        command,
        cwd=BUILD_ROOT,
        capture_output=True,
        text=True,
    )
    if completed.returncode == 0:
        if completed.stdout:
            print(completed.stdout, end="")
        if completed.stderr:
            print(completed.stderr, end="", file=sys.stderr)
        return

    print("\nPlatformIO failed:", file=sys.stderr)
    print(f"Return code: {completed.returncode}", file=sys.stderr)
    print("stdout:", file=sys.stderr)
    print(completed.stdout or "<empty>", end="\n", file=sys.stderr)
    print("stderr:", file=sys.stderr)
    print(completed.stderr or "<empty>", end="\n", file=sys.stderr)
    raise subprocess.CalledProcessError(
        completed.returncode,
        command,
        output=completed.stdout,
        stderr=completed.stderr,
    )


def generate_application_prototypes(source: str) -> str:
    pattern = re.compile(
        r"(?m)^\s*(?:[A-Za-z_]\w*\s+)*[A-Za-z_]\w*\s+([A-Za-z_]\w*)\s*"
        r"\(([^{};]*)\)\s*(?:const\s*)?\{"
    )
    prototypes = []
    for match in pattern.finditer(source):
        name = match.group(1)
        if name in {"setup", "loop"}:
            continue
        signature_start = source.rfind("\n", 0, match.start()) + 1
        signature = source[signature_start:match.end()].rsplit("{", 1)[0].strip()
        if "=" not in signature:
            prototypes.append(f"{signature};")
    return "\n".join(dict.fromkeys(prototypes))


def stage_sources():

    if not SKETCH.exists():
        raise FileNotFoundError(
            f"Sketch not found: {SKETCH}"
        )

    if BUILD_ROOT.exists():
        shutil.rmtree(BUILD_ROOT)

    (BUILD_ROOT / "src" / "application").mkdir(
        parents=True
    )
    write_platformio_wrapper()

    shutil.copy2(
        SKETCH,
        BUILD_ROOT / "src" / "application" / SKETCH.name,
    )

    for source_directory in ("board", "capabilities", "dispatcher", "errors", "protocol", "services"):
        shutil.copytree(
            ROOT / "trt-core" / "core" / source_directory,
            BUILD_ROOT / "src" / "core" / source_directory,
        )
    shutil.copytree(ROOT / "trt-core" / "transports", BUILD_ROOT / "src" / "transports")
    shutil.copytree(ROOT / "trt-core" / "core" / "config", BUILD_ROOT / "src" / "core" / "config")
    shutil.copytree(
        ROOT / "boards" / "arduino" / "runtime",
        BUILD_ROOT / "src" / "trt_runtime",
    )

    application_serial = "Serial1" if REDIRECT_APPLICATION_SERIAL_TO_SERIAL1 else "trt_application_serial"
    application_source = SKETCH.read_text(encoding="utf-8")
    application_prototypes = generate_application_prototypes(application_source)

    wrapper = f"""#include "trt_runtime/trt_runtime.h"
#include "trt_runtime/arduino_application_serial.h"

{application_prototypes}

#define Serial {application_serial}
#define setup trt_application_setup
#define loop trt_application_loop

#include "application/{SKETCH.name}"

#undef Serial
#undef setup
#undef loop

extern "C" void setup() __attribute__((used, externally_visible));
extern "C" void loop() __attribute__((used, externally_visible));

extern "C" void setup()
{{
    trt_runtime_setup();
    trt_application_setup();
}}

extern "C" void loop()
{{
    trt_application_loop();
    trt_runtime_poll();
}}
"""

    (BUILD_ROOT / "src" / "main.cpp").write_text(
        wrapper,
        encoding="utf-8",
    )

    run_platformio(
        "project",
        "init",
        "--board",
        BOARD,
        "--project-option",
        "framework=arduino",
        "--project-option",
        f"upload_port={COM_PORT}",
        "--project-option",
        f"monitor_port={COM_PORT}",
        "--project-option",
        f"build_flags=-std=gnu++17 -DTRT_HAS_STL={TRT_HAS_STL} -DTRT_HAS_DYNAMIC_MEMORY={TRT_HAS_DYNAMIC_MEMORY} -DTRT_HAS_MODULES={TRT_HAS_MODULES} -DTRT_MAX_FRAME_SIZE={TRT_MAX_FRAME_SIZE}",
        "--project-option",
        "build_unflags=-flto",
        "--project-option",
        f"lib_extra_dirs={ROOT / 'TRT-arduino' / 'libraries'}",
    )


# ============================================================
# CSV REGISTRY
# ============================================================

def record_success(
    build_id: int,
    commit: str,
):

    exists = REGISTRY.exists()

    now = datetime.now()

    with REGISTRY.open(
        "a",
        newline="",
        encoding="utf-8",
    ) as registry_file:

        fields = [
            "build_id",
            "date",
            "time",
            "project",
            "board_id",
            "board_name",
            "board_type",
            "board_revision",
            "fw_version",
            "git_commit",
            "result",
        ]

        writer = csv.DictWriter(
            registry_file,
            fieldnames=fields,
        )

        if not exists:
            writer.writeheader()

        writer.writerow(
            {
                "build_id": f"{build_id:06d}",
                "date": now.date().isoformat(),
                "time": now.strftime("%H:%M:%S"),
                "project": PROJECT,
                "board_id": f"{BOARD_ID:03d}",
                "board_name": BOARD_NAME,
                "board_type": BOARD_TYPE,
                "board_revision": BOARD_REVISION,
                "fw_version": FW_VERSION,
                "git_commit": commit,
                "result": "SUCCESS",
            }
        )


# ============================================================
# MAIN
# ============================================================

def main():

    build_id = next_build_id()

    commit = git_commit()

    try:
        stage_sources()

        write_metadata(build_id)

        run_platformio(
            "run",
            "-t",
            "upload",
        )

    except Exception as error:

        print(
            f"\nTRT build/upload failed:\n{error}"
        )

        print(
            f"\nBuild ID {build_id:06d} NOT consumed."
        )

        return 1

    record_success(
        build_id,
        commit,
    )

    print("\n-----------------------------------")
    print("TRT Build Information")
    print("-----------------------------------")
    print(f"Project        : {PROJECT}")
    print(f"Build ID       : {build_id:06d}")
    print(f"Board ID       : {BOARD_ID:03d}")
    print(f"Board Type     : {BOARD_TYPE}")
    print(f"Version        : {FW_VERSION}")
    print(f"Port           : {COM_PORT}")
    print("Build Result   : SUCCESS")
    print("Upload Result  : SUCCESS")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())