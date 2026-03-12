# STM32 Switch Table for SZEmission

The switch table is the "brain" of the car, used for controlling most functions like lights, wiper, and brake. It processes the inputs from the switches, steering wheel and pedal then compiles them into it's state and sends that out over the CAN bus. It can also send motor control commands when the override switch is turned off.

## Features

1. **State Broadcasting**: Sends a CAN message every 50ms about its own state (switch positions, throttle position)
2. **Wiper Control**: Controls the wiper converter and servo with PWM
4. **Throttle Input**: Reads and filters the analog potentiometer on the throttle pedal
5. **Motor Control**: Optionally sends motor control CAN messages 

## Test hardware

Nucleo C092RC Development Board
- **Microcontroller**: STM32C092RCTx (LQFP64)
- **Clock**: Internal HSI oscillator running at 48MHz
- **CAN Interface**: FDCAN1 using the on-board CAN transceiver
- **Programmer/Debugger**: Nucleo built-in ST-Link 

## Communication Protocols

This controller uses 4 types of IO to achieve its functionality:

- **CAN bus** for communicating with the various components of the car
- **GPIO** to read switch positions and control some outputs
- **ADC** (Analog to Digital conversion) to read the throttle pedal potentiometer
- **PWM** for controlling the wiper servo motor

### Pin Configuration

#### CAN Bus

| Signal    | Pin | Direction |
|-----------|-----|-----------|
| FDCAN1_RX | PC2 | Input     |
| FDCAN1_TX | PC3 | Output    |

#### GPIO Inputs (Switches)

| Function                     | Name           | Pin  | Type          |
|------------------------------|----------------|------|---------------|
| Switch used to enable lights | SW_LIGHTS      | PC8  | Digital Input |
| Wiper on/off                 | SW_WIPER       | PC6  | Digital Input |
| Hazard signal                | SW_HAZARD      | PC13 | Digital Input |
| Autonomous mode switch       | SW_AUTO        | PC11 | Digital Input |
| Motor control override       | SW_MC_OW       | PC7  | Digital Input |
| Headlight switch             | SW_HEADLIGHT   | PC0  | Digital Input |
| Shell relay input            | IN_SHELL_RELAY | PC1  | Digital Input |
| Brake pedal input            | IN_BRAKE       | PC12 | Digital Input |

#### GPIO Outputs

| Function               | Name                | Pin | Type           |
|------------------------|---------------------|-----|----------------|
| Wiper converter enable | OUT_WIPER_CONVERTER | PD3 | Digital Output |

#### ADC Input

| Function               | Name     | Pin |
|------------------------|----------|-----|
| Throttle Potentiometer | ADC1_IN0 | PA0 |

#### PWM Output

| Function            | Timer | Channel | Pin |
|---------------------|-------|---------|-----|
| Wiper Servo Control | TIM2  | CH1     | PA5 |

### CAN Bus Messages

#### Received Messages

| ID    | ID Type  | Name                | Purpose                                                         |
|-------|----------|---------------------|-----------------------------------------------------------------|
| **0x185** | Standard | Sync State          | No idea                                                         |
| **0x190** | Standard | Steering Wheel      | State of buttons and position of switches on the steering wheel |
| **0x123** | Standard | RPM Measurement     | RPM data from the posigion sensor                               |

##### Steering Wheel State (0x190) Bit Fields

```
Byte 0 (STW_STATE_A):
  Bit 0: ACC (Cruise Control Activate)
  Bit 1: DRIVE (Drive Mode)
  Bit 2: REVERSE (Reverse Mode)
  Bit 3: LAP (Lap timing signal)
  Bit 4: TS_L (Traction Control Left)
  Bit 5: TS_R (Traction Control Right)
  Bit 6: FUNCTION1 (Reserved)
  Bit 7: FUNCTION2 (Reserved)

Bytes 1-3: Position of the 3 switches on the steering wheel (STW_STATE_B/C/D)
```

#### Transmitted Messages

| ID    | Type     | Purpose       | Data Format                                                   | Rate |
|-------|----------|---------------|---------------------------------------------------------------|------|
| **0x129** | Standard | VCU State     | Bytes 0-1: VCU_STATE_A/B, Bytes 2-5: Throttle % (fixed-point) | 20Hz |
| **0xA51** | Extended | Motor Control | Bytes 0-3: Throttle reference (fixed-point, signed)           | 20Hz |

##### VCU State (0x129) Bit Fields

```
Byte 0 (VCU_STATE_A):
  Bit 0: HEADLIGHT
  Bit 1: HAZARD
  Bit 2: AUTO
  Bit 3: BRAKE
  Bit 4: LIGHTS_ENABLE
  Bit 5: MC_OW (Motor Control Override)
  Bit 6: WIPER
  Bit 7: Reserved

Byte 1 (VCU_STATE_B):
  Bit 0: RELAY_NO
  Bit 1: RELAY_NC
  Bits 2-7: Reserved
```

## Code Structure

This code is designed to minimize the amount of user code in generated files. The main working logic is in `user.c` and `user.h`.

### Directory Breakdown

```
sw_table_stm32/
├── sw_table_stm32.ioc            # STM32CubeIDE pin/peripheral configuration
├── sw_table_stm32.pdf            # Auto-generated hardware pinout diagram
├── Core/
│   ├── Inc/
│   │   ├── main.h               # Generated header (pin definitions)
│   │   └── user.h               # User-defined functions and structures
│   ├── Src/
│   │   ├── main.c               # Main entry point and initialization
│   │   └── user.c               # User-defined functions (motor control, CAN, GPIO)
```

### Key Files

- **main.c**: System initialization, timer setup, interrupt handlers and main loop entry point
- **user.c**: Core application logic including CAN communication, GPIO control, and throttle processing
- **user.h**: Data structures, function declarations, and most importantly declarations for constants

### Timer Configuration

- **TIM1**: Base timer for interval management (1ms period)
  - Used for CAN message intervals (20Hz)
  - Used for wiper control intervals (750ms)
- **TIM2**: PWM timer for wiper servo control (50Hz clock)


## Development Guidelines 

- **Use fixed-point math** instead of floating-point (no FPU)
- **Keep ISRs short**: Set flags and do the actual processing in main loop

### Power Optimization

See `TODO.md` for a detailed power optimization roadmap. Key strategies:

---

## Authors

Created by **SZEnergy Team** for Shell Eco Marathon

- **Váradi Marcell** (varma02@GitHub)
