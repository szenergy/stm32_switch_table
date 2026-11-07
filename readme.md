# STM32 Switch Table / VCU for SZEmission

The switch table or sometimes called VCU (Vehicle Control Unit) is the "brain" of our SZEmission urban concept electric vehicle. It used for controlling most of the car's functions like lights, wiper and acceleration. It processes  inputs from various sources like the onboard switches, the steering wheel, the pedals and the wheel speed sensor then sends a series of CAN messages that control other components like the motor controller.

## Tasks

1. **Throttle Input**: Reads and filters the analog potentiometer connected to the throttle pedal.
2. **Switch Input**: Reads the state of the various switches (eg. wiper, headlight, hazard, ...)
3. **Input Broadcasting**: Sends a CAN message with the switch positions as well as the throttle and brake pedal.
4. **Clutch Safety**: Syncronizes the motor speed with the wheel speed.
5. **Motor Control**: Sends throttle reference commands to the motor controller over CAN.
6. **Wiper**: Controls the windshield wiper's servo and power supply.

## How to drive the SZEmission

Firstly, the pilot needs to make sure that the car is turned on and that the **MC_OW** switch is in the off position on the switch table.

Then the drive mode can be selected using the two of the rotary switches on the bottom of the steering wheel. The **left (MODE)** one selects the mode, while the **right (SETTING)** one selects the setting within that mode (if applicable). See [#choosing-a-drive-mode](#choosing-a-drive-mode).

The next step is selecting the direction using the switch located on the top left of the steering wheel. This can be **drive (D)** or **reverse (R)** like in a car. Toggling the switch wakes up the motor controller, which takes a couple seconds, so wait 4s before continuing.

Now comes the clutch synchronization. The pilot must press ONLY THE BUTTON on the clutch pedal, this will trigger the sync process indicated by the LEDs on the side of the OLED display. Once they turn green, the clutch pedal can be pressed in fully.

The car should then start accelerating according to the selected drive mode.

### Choosing a Drive Mode

Out of the possible 8, there are currently 4 drive modes implemented in the VCU. They can be selected using the **MODE** rotary switch on the steering wheel.

It is important to note that the throttle pedal overrides the drive modes.

#### [A] Manual Mode

In this mode, the pilot can choose from predefined acceleration values with the **SETTING** selector. Setting 1 corresponds to the lowest 1/8 = 12.5% acceleration, while 8 means 8/8 = 100%. For simplicity, here is a table of the possible settings:

| A-1         | A-2         | A-3         | A-4         | A-5         | A-6         | A-7         | A-8         |
|-------------|-------------|-------------|-------------|-------------|-------------|-------------|-------------|
| 1/8 = 12.5% | 2/8 = 25%   | 3/8 = 37.5% | 4/8 = 50%   | 5/8 = 62.5% | 6/8 = 75%%  | 7/8 = 87.5% | 8/8 = 100%  |

#### [B] Manual Strategy Mode

This mode uses the current wheel RPM to calculate the optimal throttle.

#### [C] Automatic Strategy Mode

This is similar to mode B, but now the VCU uses a Simulink model and takes into consideration multiple variables to determine when and how much to accelerate.

#### [D] Speed Hold Mode

This is a simple cruise control where the VCU tries to maintain a the selected speed. The **SETTING** rotary switch position determines the target speed from 5km/h to 40km/h in 5km/h increments.

| D-1   | D-2    | D-3    | D-4    | D-5    | D-6    | D-7    | D-8    |
|-------|--------|--------|--------|--------|--------|--------|--------|
| 5km/h | 10km/h | 15km/h | 20km/h | 25km/h | 30km/h | 35km/h | 40km/h |

### Emergency clutch safety override

If some of the sensors ensuring the safe operation of the clutch are broken or otherwise non functional, the car will be unable to accelerate due to the built in safety features that would prevent damage to the clutch. However in extreme circumstances these can be disabled using the **MC_OW** switch. If it is flipped into the ON position, press the throttle pedal to start the syncronization. Wait around 1-2s then press down fully on the clutch pedal and now the **ACC** button on the steering wheel overrides the limit switch so the VCU will think everything is normal and start accelerating.

## Development

The C codebase is designed to be as configurable as possible even for non-programmers. The `Core/Inc/user.h` file contains all constants used in the project, which have been set for the current verison of the vehicle.

#### Timers

- TIM2 is used to hardware trigger the ADC
- TIM3 is for PWM generation to the wiper servo, on CH1 (pin PB4)
- TIM14 is the tick timer that fires every 50ms (20Hz) to wake up the CPU

#### Analog to digital

ADC1 with IN0 on pin PA0 is used to read the throttle pedal position. It is started in DMA mode with the TIM2 timer triggering it every 5ms. It writes the converted values in circular mode to the `throttle_adc_buffer` variable which is a 10 element array.

#### UART

The UART1 is connected to an FTDI chip then to a Micro USB port which acts as a simple serial interface. This is currently used for some debugging, but future plans include updating the firmware, lookup tables, strategy, or drive modes from here.

### Code Structure

```
sw_table_stm32/
├── sw_table_stm32.ioc           # STM32CubeMX pin/peripheral configuration
├── readme.md                    # You are reading this file right now
├── license                      # MIT License file
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

- Use single precision or fixed point math.
  - The STM32F412RET6 has an FPU that only supports single precision (float) values.
- Avoid interrupts and use hardware features where possible.
  - Since power efficiency is a primary concern, the code is designed to minimize CPU usage.
- Use structs and easily understandable variable names.
- Do not hardcode values! If you need to use a constant value, define it in `user.h` with a descriptive name.

---

Created by **SZEnergy Team** for Shell Eco Marathon

- **Váradi Marcell** (varma02@GitHub)

This project is licensed under the [MIT License](license).
