# STM32 Switch Table for SZEmission

The switch table is the "brain" of our SZEmission electric vehicle, used for controlling functions like lights, wiper and acceleration. It processes the inputs from switches, the steering wheel and the throttle pedal and based on that sends a series of CAN messages. The most important role of the switch table currently is calculating the thorottle reference for the motor based on the pilot's inputs and various strategy algorithms.

## Tasks

1. **Throttle Input**: Reads and filters the analog potentiometer connected to the throttle pedal.
2. **Switch Input**: Reads the state of the various switches (eg. wiper, headlight, hazard, ...)
3. **Input Broadcasting**: Sends a CAN message with the switch positions as well as the throttle and brake pedal.
4. **Wiper Control**: Controls the windshield wiper's servo and power supply.
5. **Motor Control**: Sends throttle reference commands to the motor controller over CAN.  
   This function is turned off when either MC_OW (Motor Control Override) or AUT (Autonomous Mode) switch is active.

## Drive Modes

The drive mode can be selected by the pilot using the two rotary switches on the steering wheel. The one on the **left (ROT3)** selects the drive mode, and the one on the **right (ROT1)** selects the setting within that mode. The calculated reference only influences the **accelerate (ACC)** button's behavior, the throttle pedal always overrides these settings. However, for any throttle output, first the direction selector switch has to be in **drive (D)** or **reverse (R)**. The **neutral (N)** setting inibits all acceleration.

All modes, including the pedal, are rate limited to prevent sudden changes in acceleration. This is currently set to 5% per 50 milliseconds up and 25%/50ms down. This means the throttle takes at least 1 second to go from 0% to 100% and 0.2 seconds to go down from 100% to 0%.

This table describes the drive modes and their settings:

| Setting / Mode | A: Manual     | B: Manual Strategy | C: Automatic Strategy | D: Speed Hold |
| -------------- | ------------- | ------------------ | --------------------- | ------------- |
| 1              | 80% throttle  | Z22 ?              | LTV - LQR             | 5 km/h        |
| 2              | 90% throttle  | Z24 ?              | LQR + Switching       | 10 km/h       |
| 3              | 100% throttle | Aumovio Z22        | Hybrid                | 15 km/h       |
| 4              | -             | -                  | -                     | 20 km/h       |
| 5              | -             | -                  | -                     | 25 km/h       |
| 6              | -             | -                  | -                     | 30 km/h       |
| 7              | -             | -                  | -                     | 35 km/h       |
| 8              | -             | -                  | -                     | 40 km/h       |

### Manual Mode

In manual mode, the pilot can chose between 3 pre-defined throttle values that are applied when the **accelerate ACC** button is pressed.

### Manual Strategy Mode

In this mode the pilot can chose what strategy's calculated throttle reference to apply when the **accelerate ACC** button is pressed. These are precalculated values based on wheel RPM or speed. The driver still has to chose where to accelerate in this mode.

### Automatic Strategy Mode

As the name suggests, in this mode the switch table calculates takes into consideration multiple variables and previous simulation results to derermain when and how much to accelerate. The pilot's only job is to hold down the **accelerate (ACC)** button continously which acts as a "dead man's switch" in this mode. This tells the system that it can accelerate when the conditions are right.

### Speed Hold Mode

This is a simple cruise control where the switch table tries to maintain a constant speed selected by the pilot. After selecting the speed, holding down the **accelerate (ACC)** button tells the switch table that the pilot is ready to accelerate.

## Implementation

The following sections describe the hardware and software implementation of the switch table. It is intended for developers who want to understand the codebase. If you are just a user you can stop reading now.

The hardware of the switch table is a custom designed PCB with a lot of ports and 6 switches.

- **Custom board:** Switch Table v4
- **Microcontroller**: STM32F412RET6 (LQFP64)
- **Clock**: 8MHz external crystal, HSE
- **CAN Interface**: CAN2 using an ESDCAN24-2BLY transceiver
- **Micro USB port**: UART1 converted to USB serial using an FTDI chip
- **Programmer/Debugger**: External ST-Link programmer with an SWD TC2030-IDC connector

The software is a combination of C code for basic functionality and MATLAB Simulink for the complex strategy algorithms. The Simulink subsystems are generated into C code as reusable functions with the Embedded coder extension and then integrated into the project in STM32CubeIDE. The models are in a different repository, called [sw-table-stm32-simulink](https://github.com/szenergy/sw-table-stm32-simulink). There is also a monorepo that ties these projects together at [szenergy/SZEnergy_BE_CTRL](https://github.com/szenergy/SZEnergy_BE_CTRL).

### Configuration

The C codebase is designed to be configurable even for non-programmers. Open the `Core/Inc/user.h` file and on the top are all constants used by the project, which have been set for the current verison of the vehicle.

### Peripherals

Here it is described in detail how the various peripherals of the `STM32F412RET6` microcontroller are used by this project.

#### CAN bus

The CAN2 peripheral is connected to the ESDCAN transciever then to the vehicle's CAN bus network. There are many devices and messages on the bus, but the switch table only interacts with a few of them.

| Direction | ID    | Name                         | Description                                |
| --------- | ----- | ---------------------------- | ------------------------------------------ |
| Received  | 0x190 | Steering_Wheel               | Button and switch states                   |
| Received  | 0x123 | Encoder_Messages             | Wheel RPM measurement                      |
| Sent      | 0xA51 | VESC_Relativ_Current_Command | Torque reference for the motor controller  |
| Sent      | 0x129 | VCU_Status_Message           | Switch positions and filtered throttle     |
| Sent      | 0x150 | VCU_Calculated_State         | Lap number, time, distance and drive state |
| Sent      | 0x151 | VCU_Simulink_Debug           | Simulink model debug values                |

Further details on these CAN bus messages can be found in the current [CAN dbc file](https://github.com/szenergy/SZEmission_CANdb). Two important signals to note are in the **VCU_Calculated_State** message for drive mode and setting. Here is an extended version of the drive mode and setting table that shows what the values on the CAN bus mean, where the columns represent the drive modes and the rows are the settings within those modes:

| Setting / Mode | 0 : Neutral | 1 : Manual     | 2 : Manual Strategy | 3 : Automatic Strategy | 4 : Speed Hold |
| -------------- | ----------- | -------------- | ------------------- | ---------------------- | -------------- |
| 0              | Neutral     | Pedal override | -                   | -                      | -              |
| 1              | -           | 80% throttle   | Z22 ?               | LTV - LQR              | 5 km/h         |
| 2              | -           | 90% throttle   | Z24 ?               | LQR + Switching        | 10 km/h        |
| 3              | -           | 100% throttle  | Aumovio Z22         | Hybrid                 | 15 km/h        |
| 4              | -           | -              | -                   | -                      | 20 km/h        |
| 5              | -           | -              | -                   | -                      | 25 km/h        |
| 6              | -           | -              | -                   | -                      | 30 km/h        |
| 7              | -           | -              | -                   | -                      | 35 km/h        |
| 8              | -           | -              | -                   | -                      | 40 km/h        |

#### Timers

- TIM2 is used to hardware trigger the ADC
- TIM3 is for PWM generation to the wiper servo, on CH1 (pin PB4)
- TIM14 is the tick timer that fires every 50ms (20Hz) to wake up the CPU

#### Analog to digital

ADC1 with IN0 on pin PA0 is used to read the throttle pedal position. It is started in DMA mode with the TIM2 timer triggering it every 5ms. It writes the converted values in circular mode to the `throttle_adc_buffer` variable which is a 10 element array.

#### UART

The UART1 is connected to an FTDI chip then to a Micro USB port which acts as a simple serial interface. This is currently used for some debugging, but future plans include updating the firmware, lookup tables, strategy, or drive modes from here.

### Pin Configuration

![Picture of the STM32F412RET6 from CubeMX](./docs/sw_table_v4_pin_config.png)

| Function                     | Name                         | Pin  | Type         |
| ---------------------------- | ---------------------------- | ---- | ------------ |
| Switch used to enable lights | Lights_enable_switch         | PC3  | GPIO Input   |
| Wiper on/off                 | Wiper_switch                 | PB0  | GPIO Input   |
| Hazard signal                | Hazard_switch                | PB5  | GPIO Input   |
| Autonomous mode switch       | Autonomous_switch            | PC4  | GPIO Input   |
| Motor control override       | Motorcontrol_override_switch | PC5  | GPIO Input   |
| Headlight switch             | Headlight_switch             | PC13 | GPIO Input   |
| Brake pedal input            | Brake_pedal_input            | PA9  | GPIO Input   |
| Brake pedal input            | Clutch_pedal_input           | PA8  | GPIO Input   |
| Wiper converter enable       | OUT_WIPER_CONVERTER          | PD3  | GPIO Output  |
| Throttle Potentiometer       | Throttle_pedal_ADC           | PA0  | ADC Input    |
| Wiper Servo Control          | Wiper_PWM                    | PB4  | TIM3 PWM CH1 |
| Yellow debug LED             | LED1_Yellow                  | PA1  | GPIO Output  |
| Red error LED                | LED2_Red                     | PA2  | GPIO Output  |
| Green status LED             | LED3_Green                   | PA3  | GPIO Output  |
| Blue debug LED               | LED4_Blue                    | PA4  | GPIO Output  |
| CAN Receive                  | CAN2_RX                      | PB12 | CAN2_RX      |
| CAN Transmit                 | CAN2_TX                      | PB13 | CAN2_TX      |
| UART Receive                 | USART1_RX                    | PB7  | USART1_RX    |
| UART Receive                 | USART1_TX                    | PB6  | USART1_TX    |

### Code Structure

```
sw_table_stm32/
├── sw_table_stm32.ioc           # STM32CubeMX pin/peripheral configuration
├── readme.md                    # You are reading this file right now
├── license                      # MIT License file
├── docs/                        # Images and other files required for the readme
├── Core/                        # Source code folder
│   ├── Inc/                     # Header files
│   │   ├── Simulink/            # Simulink generated code, do not edit this directly
│   │   ├── main.h               # Generated header, the pin definitions from the ioc are here
│   │   └── user.h               # The file for user definitions, data structures and function prototypes
│   ├── Src/                     # Source files
│   │   ├── Simulink/            # Simulink generated code, do not edit this directly
│   │   ├── main.c               # Main entry point and peripheral initialization, imports and calls user code
│   │   └── user.c               # User-defined functions, variables and controller logic
```

#### Key Files

- **main.c**: Mostly auto-generated code for initializing the peripherals and the main loop that calls the user code. It also contains a timer interrupt handler.
- **user.c**: Core application logic devided into functions. This is where inputs are read, processed and CAN messages sent.
- **user.h**: Data structures, type definitions, public function prototypes and configurable constants are defined here.

### Development Guidelines

- **Use single precision or fixed point math.**
  - The STM32F412RET6 has an FPU but it only supports single precision (float) values.
- **Avoid interrupts and use hardware features where possible.**
  - Since power efficiency is a primary concern, therefor the code is designed to minimize CPU usage.
  - This means using timers that hardware trigger other peripherals and DMA to handle saving of results.
- **Use structs and understandable variable names.**
  - The code should be easy to read and understand.
  - This is especially important for the next generation of students in the team.
- **Use enums, defined constants, bitfields for better readability.**
  - Since this code is likely to be read by people with varying levels of programming experience, it is important to make it as clear as possible what the code is doing.
  - DO NOT HARDCODE VALUES! If you need to use a constant value, define it in `user.h` with a descriptive name.
- **Add comments where the intention might not be clear.**
  - If you are doing something that might not be immediately obvious to someone else, add a comment explaining why you are doing it.

---

Created by **SZEnergy Team** for Shell Eco Marathon

- **Váradi Marcell** (varma02@GitHub)

This project is licensed under the [MIT License](license).
