#!/usr/bin/env python3

from __future__ import annotations

import csv
import json
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

REDIRECT_APPLICATION_SERIAL_TO_SERIAL1 = True

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

#include <cstdint>

namespace trt_profile {{

constexpr uint16_t kBoardId = {BOARD_ID};
constexpr uint32_t kBuildId = {build_id};

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

    shutil.copy2(
        SKETCH,
        BUILD_ROOT / "src" / "application" / SKETCH.name,
    )

    serial_redirect = (
        "#define Serial Serial1\n"
        if REDIRECT_APPLICATION_SERIAL_TO_SERIAL1
        else ""
    )

    serial_restore = (
        "#undef Serial\n"
        if REDIRECT_APPLICATION_SERIAL_TO_SERIAL1
        else ""
    )

    wrapper = f"""
#define setup trt_application_setup
#define loop trt_application_loop

{serial_redirect}
#include "application/{SKETCH.name}"
{serial_restore}

#undef setup
#undef loop

void setup()
{{
    trt_application_setup();
}}

void loop()
{{
    trt_application_loop();

    // future:
    // TRT runtime poll
}}
"""

    (BUILD_ROOT / "src" / "trt_wrapper.cpp").write_text(
        wrapper,
        encoding="utf-8",
    )

    subprocess.run(
        [
            sys.executable,
            "-m",
            "platformio",
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
            "build_flags=-std=gnu++17",
        ],
        cwd=BUILD_ROOT,
        check=True,
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

        subprocess.run(
            [
                sys.executable,
                "-m",
                "platformio",
                "run",
                "-t",
                "upload",
            ],
            cwd=BUILD_ROOT,
            check=True,
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