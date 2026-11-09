# Keypad Based Product Counting and Automatic Batch Packing System with Station Sensing using LPC2129

## Overview
An Embedded C conveyor-based system using the NXP LPC2129. The operator enters a batch number and target product quantity through a 4×4 keypad. Two IR sensors monitor product movement from the conveyor entry to the packing station. The LCD displays prompts, count and status; LEDs and a buzzer indicate completion or faults.

## Hardware
- LPC2129 (ARM7TDMI-S) microcontroller
- 4×4 matrix keypad
- Two IR sensors (IR1 and IR2)
- DC motor with L293D motor driver
- LCD, red/green LEDs and buzzer

## Main Operation
1. Enter the batch number and required quantity using the keypad; press `S` to start.
2. The conveyor motor starts.
3. IR1 detects a product and triggers EINT0, starting Timer1.
4. IR2 detects the product at the packing station through EINT1 and stops Timer1.
5. The main program increments the count after both sensor flags are set.
6. The sequence repeats until the requested quantity is reached.
7. If Timer1 times out before IR2 confirms arrival, the motor stops and a belt-fault indication is given. On completion, the motor stops and the batch-complete indication is displayed.

## Software Files
Files and their Purpose
`mainDriver1.c` => Main application, GPIO, interrupts, motor control, counting and batch handling
`keypad.h` & `keypadTest.c` => Keypad scanning, numeric entry and standalone keypad test
`lcdHeader.h` => LCD initialization and display functions
`delay.h` => Timer0-based delay functions
`testIRCode.c` => Standalone IR sensor test
`dcMotorTest.c` => Standalone motor test

## Key Design Points
- EINT0 and EINT1 handle IR sensor events; Timer1 provides the product-arrival timeout.
- Timer0 is used for general software delays.
- Keypad scanning uses a 250 ms delay and waits for key release.
- EINT1 is disabled after an accepted IR2 event and re-enabled after the sensor output returns HIGH, preventing repeated servicing while the sensor remains active.
- `volatile` flags communicate events from interrupt service routines to the main loop.
- IR sensor, keypad and motor modules were tested separately before integration.

## Development Setup
Keil µVision 4 with ARMCC; Proteus Professional 8 for simulation; Flash Magic for programming the LPC2129.

**Timing assumption:** The current Timer1 timeout and Timer0 delay calculations assume PCLK = 15 MHz. Confirm the board clock configuration and hardware pin connections before testing.
