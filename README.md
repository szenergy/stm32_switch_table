# STM32 Switch Table for SZEmission

The switch table is the "brain" of the car, used for controlling most functions like lights, wiper and brake. It can also send motor control commands, while receiving inputs for acceleration and drive mode from the steering wheel but this function can be turned off with a switch.

## Functions

1. Sends a CAN message every 20ms about its own state _(VCU state)_ which includes the position of every switch
2. Controls the wiper with a PWM signal
3. Controls the lights, brake and shell relay ? with GPIO
4. Reads an analog potentiometer
5. Optionally _(if mc_ow switch is off)_ sends motor control CAN messages

## Communication

This controller uses 4 types of IO to achive its functionality.

- **CAN bus** for communicating with the various other systems in the car
- **GPIO** to read the position of the switches
- **ADC** (Analog to Digital) conversion to read a potentiometer
- **PWM** for controlling the wiper servo

### Pin configuration

#### CAN

- **FDCAN1_RX** -> PC2
- **FDCAN1_TX** -> PC3

#### GPIO

- **Switch for lights** (SW_LIGHTS) -> PC8
- **Switch for wiper** (SW_WIPER) -> PC6
- **Switch for hazards** (SW_HAZARD) -> PC13
- **Switch for brake** (SW_BRAKE) -> PC12
- **Switch for autonomous mode** (SW_AUTO) -> PC11
- **Switch for MC override** (SW_MC_OW) -> PC7
- **Input function** ? (IN_FUNCTION) -> PC0
- **Input for shell c1** ? (IN_SHELL_C1) -> PC1
- **Output for brake** (OUT_BRAKE) -> PD0
- **Output for shell relay** ? (OUT_SHELL_RELAY) -> PD1

#### ADC

- **Potentiometer input** (ADC1_IN0) -> PA0

#### PWM

- **Wiper control output** (TIM2_CH1) -> PA5

### CAN bus

These are the different CAN messages that this controller handles.

#### Receiving

- **0x185** can sync state ?
- **0x190** steering wheel state
- **0x123** RPM measurement
- **0x10** Shell relay ?

#### Transmitting

- **0x129** VCU _(switch table)_ state
- **0xA51** motor control

## Code structure

This code is designed in a way to minimize user code in generated files. So the main working parts of the program are in the `user.c` and `user.h` files.

#### Directory breakdown

- `sw_table_stm32.ioc` - Pin and peripheral configuration file for STM32CubeIDE
- `Drivers/` - Contains the Hardware Abstraction Layer (HAL) and other device specific generated code
- `Core/` - This is the main source folder
  - `Inc/` - Header files
    - `main.h` - Generated header file for main.c
    - `user.h` - User defined functions and variables
    - other generated peripheral header files
  - `Src/` - Source files
    - `main.c` - Contains the main function which initializes the system and starts the main loop
    - `user.c` - Implementation of user defined functions and variables
    - other generated peripheral source files
  - `Startup/` - Generated startup files for the microcontroller

---

### Authors

Created by _SZEnergy Team_ for Shell Eco Marathon.

- Váradi Marcell _(varma02@Github)_
