# Development Environment Setup

## Scope

This repository contains C++ TRT-core sources, Arduino application sketches, and the `tools/upload.py` deployment workflow. The uploader generates an isolated PlatformIO project in `.trt-build/<project>` and does not modify the original sketch.

## Required Software

### Python

- Python 3.10 or newer is required for `tools/upload.py`.
- Use the same Python interpreter for the uploader, PlatformIO, `truststore`, and `python-certifi-win32`.

Verify the active interpreter:

```powershell
python --version
python -c "import sys; print(sys.executable)"
```

### Python Packages

`tools/upload.py` uses only Python standard-library modules plus `truststore` through its generated PlatformIO launcher. Install the complete required set:

```powershell
python -m pip install --upgrade pip
python -m pip install platformio truststore python-certifi-win32
```

Verify each package with the uploader's interpreter:

```powershell
python -m platformio --version
python -c "import truststore; print(truststore.__file__)"
python -c "import certifi_win32; print(certifi_win32.__file__)"
```

`csv`, `json`, `pathlib`, `re`, `shutil`, `subprocess`, `sys`, and `datetime` are standard-library modules and require no installation.

### PlatformIO Requirements

- PlatformIO Core 6 or newer.
- Network access on the first build so PlatformIO can obtain the target platform, framework, toolchain, and declared Arduino libraries.
- A PlatformIO board ID configured in `tools/upload.py`, for example `uno` or `megaatmega2560`.
- The selected board must support the generated Arduino framework project.
- An accessible upload port configured by `COM_PORT`.

PlatformIO is invoked only through the generated `.trt-build/<project>/platformio_wrapper.py`; do not depend on a `pio` executable being present on `PATH`.

## Build and Upload

Configure `COM_PORT`, `PROJECT`, `BOARD`, `BOARD_ID`, `FW_VERSION`, `CAPABILITIES`, and serial settings at the top of `tools/upload.py`, then run:

```powershell
python tools/upload.py
```

The command stages source files, generates board metadata and the application wrapper, initializes PlatformIO, builds, and uploads. A row is added to `build_registry.csv` only after PlatformIO returns success. Failed builds and uploads do not consume a build ID.

## PlatformIO SSL / Corporate Network Troubleshooting

### Symptoms

On a machine behind corporate SSL inspection, PlatformIO may fail during project initialization or package download with one or more of:

```text
HTTPClientError
certificate verify failed
self-signed certificate in certificate chain
```

The direct Python symptom is:

```text
SSL: CERTIFICATE_VERIFY_FAILED
```

### Diagnosis

First verify the active interpreter and package installation:

```powershell
python -c "import sys; print(sys.executable)"
python -c "import truststore; print(truststore.__file__)"
python -c "import certifi_win32; print(certifi_win32.__file__)"
```

Test the registry with normal Python requests. On the affected network, this can fail with `SSL: CERTIFICATE_VERIFY_FAILED`:

```powershell
python -c "import requests; print(requests.get('https://registry.platformio.org', timeout=30).status_code)"
```

Test again after injecting the Windows operating-system trust store. This is the verified workaround; it should return HTTP `200`:

```powershell
python -c "import truststore; truststore.inject_into_ssl(); import requests; print(requests.get('https://registry.platformio.org', timeout=30).status_code)"
```

### Resolution

Install both certificate integration packages in the Python environment that runs `upload.py`:

```powershell
python -m pip install --upgrade truststore python-certifi-win32
```

`upload.py` writes this launcher for every generated build:

```python
import truststore

truststore.inject_into_ssl()

from platformio.__main__ import main

raise SystemExit(main())
```

All PlatformIO commands must run through that launcher. The uploader prints the full command before execution and, on failure, prints the PlatformIO return code, stdout, and stderr. To reproduce the initialization through the launcher after staging:

```powershell
python .trt-build\<project>\platformio_wrapper.py project init --board <board-id>
```

If the truststore-based `requests` test returns `200` but PlatformIO still fails, retain the uploader's printed stdout and stderr and provide them to the network or security team. Do not disable certificate verification; the solution is to trust the corporate root certificate through the operating-system certificate store.