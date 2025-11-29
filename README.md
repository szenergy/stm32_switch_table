# STM32 Switch Table for SZEmission

The switch table is the "brain" of the car, used for controlling most functions like lights, wiper, and brake. It processes the inputs from the switches, steering wheel and pedal then compiles them into it's state and sends that out over the CAN bus. It can also send motor control commands when the override switch is turned off.

## Features

- **State Broadcasting**: Sends a CAN message az 20Hz about its own state (switch positions, throttle position)
- **Wiper Control**: Controls the wiper converter and servo with PWM
- **Throttle Input**: Reads and filters the analog potentiometer on the throttle pedal
- **Motor Control**: Optionally sends motor control CAN messages

## Communication Protocols

This controller uses 4 types of IO to achieve its functionality:

- **CAN bus** for communicating with the various components of the car
- **GPIO** to read switch positions and control some outputs
- **ADC** (Analog to Digital conversion) to read the throttle pedal potentiometer
- **PWM** for controlling the wiper servo motor

### CAN Bus Messages

#### Received Messages

| ID        | ID Type  | Name            | Purpose                                                         |
| --------- | -------- | --------------- | --------------------------------------------------------------- |
| **0x185** | Standard | Sync State      | myRIO status message (used for autonomous driving)              |
| **0x190** | Standard | Steering Wheel  | State of buttons and position of switches on the steering wheel |
| **0x123** | Standard | RPM Measurement | RPM data from the encoder (motor position sensor)               |

##### Steering Wheel State (0x190) Bit Fields

```
Byte 0 (STW_STATE_A):
  Bit 0: ACC (Accelerate using predefined speed profile)
  Bit 1: DRIVE (Drive Mode)
  Bit 2: REVERSE (Reverse Mode)
  Bit 3: LAP (End of lap button)
  Bit 4: TS_L (Turn signal left)
  Bit 5: TS_R (Turn signal right)
  Bit 6: FUNCTION1 (Reserved)
  Bit 7: FUNCTION2 (Reserved)

Bytes 1-3: Position of the 3 switches on the steering wheel (STW_STATE_B/C/D)
```

#### Transmitted Messages

| ID        | Type     | Purpose       | Data Format                                                   | Rate |
| --------- | -------- | ------------- | ------------------------------------------------------------- | ---- |
| **0x129** | Standard | VCU State     | Bytes 0-1: VCU_STATE_A/B, Bytes 2-5: Throttle % (fixed-point) | 20Hz |
| **0xA51** | Extended | Motor Control | Bytes 0-3: Throttle reference (fixed-point, signed)           | 20Hz |

##### VCU State (0x129) Bit Fields

```
Byte 0 (VCU_STATE_A):
  Bit 0: HEADLIGHT
  Bit 1: HAZARD
  Bit 2: AUTO  (Autonomous driving mode)
  Bit 3: BRAKE (Brake pedal pressed)
  Bit 4: LIGHTS_ENABLE
  Bit 5: MC_OW (Disables motor control messages)
  Bit 6: WIPER
  Bit 7: Reserved

Byte 1 (VCU_STATE_B):
  Bit 0: RELAY_NO (Shell relay)
  Bit 1: RELAY_NC (Shell relay)
  Bits 2-7: Reserved
```

## Code Structure

This project is designed to minimize the amount of user code in generated files. The main working logic is in `user.c` and `user.h`.

### Directory Breakdown

```
sw_table_stm32/
├── sw_table_stm32.ioc            # STM32CubeIDE pin/peripheral configuration
├── Core/
│   ├── Inc/
│   │   ├── main.h               # Generated header (mostly for pin definitions)
│   │   └── user.h               # User definitions of functions and structures
│   ├── Src/
│   │   ├── main.c               # Entry point and initialization
│   │   └── user.c               # User-defined functions (core logic)
```

### Key Files

- **main.c**: System initialization, timer and CAN setup, interrupt handlers and main loop entry point
- **user.h**: Data structures, function declarations and constants
- **user.c**: Core application logic including CAN communication, GPIO control, and throttle ADC processing

### Timer Configuration

- **TIM1**: Base timer for interval management (1ms period)
  - Used for CAN message intervals (20Hz)
  - Used for wiper control intervals (750ms)
- **TIM2**: PWM timer for wiper servo control (50Hz clock)

## Test hardware

Nucleo C092RC Development Board

- **Microcontroller**: STM32C092RCTx (LQFP64)
- **Clock**: Internal HSI oscillator running at 48MHz
- **CAN Interface**: FDCAN1 using the on-board CAN transceiver
- **Programmer/Debugger**: Nucleo built-in ST-Link

### Pin Configuration

#### CAN Bus

| Function     | Name      | Test Pin |
| ------------ | --------- | -------- |
| CAN receive  | FDCAN1_RX | PC2      |
| CAN transmit | FDCAN1_TX | PC3      |

#### GPIO Inputs

| Function                     | Name           | Test Pin |
| ---------------------------- | -------------- | -------- |
| Switch used to enable lights | SW_LIGHTS      | PC8      |
| Wiper on/off                 | SW_WIPER       | PC6      |
| Hazard signal                | SW_HAZARD      | PC13     |
| Autonomous mode switch       | SW_AUTO        | PC11     |
| Motor control override       | SW_MC_OW       | PC7      |
| Headlight switch             | SW_HEADLIGHT   | PC0      |
| Shell relay input            | IN_SHELL_RELAY | PC1      |
| Brake pedal input            | IN_BRAKE       | PC12     |

#### GPIO Outputs

| Function                        | Name                | Test Pin |
| ------------------------------- | ------------------- | -------- |
| Wiper DC -> DC converter enable | OUT_WIPER_CONVERTER | PD3      |

#### ADC Input

| Function                     | Name     | Test Pin |
| ---------------------------- | -------- | -------- |
| Throttle pedal potentiometer | ADC1_IN0 | PA0      |

#### PWM Output

| Function            | Timer | Channel | Test Pin |
| ------------------- | ----- | ------- | -------- |
| Wiper servo control | TIM2  | CH1     | PA5      |

## Development Guidelines

- **Use fixed-point math** instead of floating-point (no FPU)
- **Keep interrupt callbacks short**: Set flags and do the actual processing in main loop

### Power Optimization

Work in progress, once final hardware is finished.

---

Created by **SZEnergy Team** for Shell Eco Marathon

- **Váradi Marcell** (varma02@GitHub)
