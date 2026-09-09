# STM32 Application Workspace

## Overview

This repository is used to develop and test independent STM32 applications inside a single STM32CubeIDE project.

The goal is to avoid creating and maintaining multiple STM32 projects. Instead, all applications live in the same project and only one application is active at a time.

Examples of future applications:

- Signal Generator
- LCD I2C Controller
- LED Ring Controller
- ADC Test
- UART Test
- PWM Test
- SPI Test
- I2C Test
- Future TRT integration

---

# Project Structure

```text
Sources/
├── app_selector.h
├── app.h
├── main.c
│
├── program1.c
├── program1.h
│
├── program2.c
├── program2.h
│
├── program3.c
├── program3.h
│
├── syscalls.c
└── sysmem.c
```

---

# Development Environment

This project is developed, built, flashed, and debugged using:

```text
STM32CubeIDE
```

The project is intended to be used directly inside STM32CubeIDE.

Do not assume:

- PlatformIO
- VSCode build tasks
- Command-line flashing
- External build systems

All normal development should be performed through STM32CubeIDE.

---

# How It Works

Only one application is active at a time.

The active application is selected in:

```c
app_selector.h
```

Example:

```c
#define PROGRAM1_ENABLED

// #define PROGRAM2_ENABLED
// #define PROGRAM3_ENABLED
```

Only the enabled program provides:

```c
app_init();
app_loop();
```

which are used by the common application entry point.

---

# Main Application Flow

The project's entry point is:

```c
main.c
```

Execution flow:

```text
main()
│
├─ app_init()
│
└─ while(1)
      app_loop()
```

Every program must implement:

```c
void app_init(void);
void app_loop(void);
```

---

# Creating a New Program

## Step 1 - Create the Header File

Example:

```text
program4.h
```

```c
#ifndef PROGRAM4_H
#define PROGRAM4_H

void app_init(void);
void app_loop(void);

#endif
```

---

## Step 2 - Create the Source File

Example:

```text
program4.c
```

```c
#ifdef PROGRAM4_ENABLED

#include "app.h"

void app_init(void)
{
}

void app_loop(void)
{
}

#endif
```

---

## Step 3 - Select the Program

Edit:

```text
app_selector.h
```

Example:

```c
// #define PROGRAM1_ENABLED
// #define PROGRAM2_ENABLED
// #define PROGRAM3_ENABLED

#define PROGRAM4_ENABLED
```

Only one program should be enabled at a time.

---

## Step 4 - Build

Inside STM32CubeIDE:

```text
Project
→ Build Project
```

---

## Step 5 - Flash

Inside STM32CubeIDE:

```text
Run
→ Debug As
→ STM32 Cortex-M C/C++ Application
```

or use the normal Run/Debug button configured for the project.

---

# Program Design Guidelines

## One Program = One Purpose

Recommended examples:

```text
program1 = Signal Generator
program2 = LCD I2C Controller
program3 = LED Ring Controller
program4 = ADC Test
program5 = UART Test
program6 = PWM Test
```

Avoid mixing unrelated functionality inside the same program.

---

## Keep Programs Independent

Each program should be self-contained.

For example:

```text
program2
```

should not depend on:

```text
program1
```

unless absolutely necessary.

---

## Leave Core Files Alone

Normally there is no need to modify:

```text
main.c
app.h
syscalls.c
sysmem.c
startup files
linker scripts
STM32CubeIDE settings
```

New functionality should be implemented by creating new program files.

---

# Using AI to Create New Applications

When using an AI assistant, always provide this document first.

After that, describe the new application.

Example:

```text
Read STM32_APPLICATION_WORKSPACE.md first.

Create a new application called program4.

Purpose:
Control a 20x4 I2C LCD display.

Requirements:

- STM32CubeIDE compatible
- Implement app_init()
- Implement app_loop()
- Do not modify main.c
- Do not modify project settings
- Use the architecture described in STM32_APPLICATION_WORKSPACE.md
```

This helps ensure all future programs follow the same architecture.

---

# Current Workflow

```text
1. Create programN.c
2. Create programN.h
3. Enable it in app_selector.h
4. Build in STM32CubeIDE
5. Flash the board
6. Test
```

No additional STM32 projects should be created unless there is a hardware-specific reason that cannot be handled inside the current project.

---

# Current Project Philosophy

This repository is a learning and experimentation workspace for STM32 development.

Applications are intentionally kept separate so they can be:

- Developed independently
- Tested independently
- Reused later
- Integrated into larger projects such as TRT-Core if needed

The applications themselves should remain independent from TRT-Core unless explicitly required by a future project.
# Build and Flash

This project is developed and loaded using STM32CubeIDE.

## Build

Open STM32CubeIDE and build the project:

```text
Project
→ Build Project
```

or use the hammer icon in the toolbar.

A successful build should complete without errors.

## Flash

Connect the STM32 board through the ST-LINK USB connection.

To program the firmware:

```text
Run
→ Debug As
→ STM32 Cortex-M C/C++ Application
```

The IDE will:

```text
Build
↓
Connect to ST-LINK
↓
Erase Flash
↓
Program Flash
↓
Start Debug Session
```

Press:

```text
Resume (F8)
```

to start firmware execution.

## Subsequent Updates

After modifying the code:

```text
1. Build Project
2. Run or Debug
```

STM32CubeIDE will automatically rebuild and reprogram the board.

## Switching Applications

Select the desired application in:

```text
Sources/app_selector.h
```

Example:

```c
#define PROGRAM2_ENABLED
```

Then:

```text
Build Project
↓
Run / Debug
```

The new application will be programmed into the STM32.

## Verification

After creating a new application:

```text
1. Enable it in app_selector.h
2. Build
3. Flash
4. Verify that the expected hardware behavior is present
```

If the program behaves as expected, the application