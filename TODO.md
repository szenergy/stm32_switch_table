# Power and Resource Optimization TODO

## Priority 1: GPIO EXTI Interrupts (30-40% power savings)
- [ ] Replace `Port_Update()` polling with GPIO EXTI interrupts
- [ ] Remove 8x `HAL_GPIO_ReadPin()` calls from main loop
- [ ] Implement interrupt handlers for switch state changes
- [ ] Update flag system to use EXTI callbacks instead of polling

## Priority 2: ADC Optimization (15-20% power savings)
- [ ] Replace polling-based ADC with interrupt-driven reads
- [ ] Remove 5ms `HAL_ADC_PollForConversion()` timeout
- [ ] Implement `HAL_ADC_ConvCpltCallback()` for single-shot conversions
- [ ] Move ADC reads to CAN interval triggers only (not every main loop)

## Priority 3: Remove LED Debugging (5-10% power savings)
- [ ] Remove `HAL_GPIO_TogglePin()` call at user.c:419 (LED_2 toggle)
- [ ] Consider conditional compilation flag for development vs production
- [ ] Keep error LED (LED_1) in `CAN_Error()` for debugging only

## Priority 4: Float Math Replacement (5% power savings)
- [ ] Replace `(double)pot_value.current / 1023 * 100000` with fixed-point math
- [ ] Update `CAN_Send_Vcu()` (user.c:276) throttle calculation
- [ ] Update `CAN_Send_Mc()` (user.c:302) throttle calculation
- [ ] Consider lookup table for throttle scaling if needed

## Priority 5: Conditional Pot Filtering (2-3% power savings)
- [ ] Skip EMA calculation in `Pot_Read_Filtered()` when throttle is zero
- [ ] Only run expensive filtering when acceleration is needed
- [ ] Add flag to detect if throttle value actually changed

## Priority 6: Wiper PWM Optimization (2% power savings)
- [ ] Keep PWM timer running continuously (don't call Start/Stop)
- [ ] Modulate wiper output by setting CCR1 to 0 instead of stopping timer
- [ ] Reduces initialization overhead per wipe cycle

## Priority 7: Clock Gating (2-3% power savings)
- [ ] Disable GPIOD clock if only using 2 pins (OUT_BRAKE, OUT_WIPER_CONVERTER)
- [ ] Audit unused peripheral clocks in RCC configuration
- [ ] Review `SystemClock_Config()` for additional optimization opportunities

## Priority 8: CAN Bus Idle Optimization (2-3% power savings)
- [ ] Check FDCAN datasheet for sleep/standby modes
- [ ] Implement FDCAN low-power mode between message intervals
- [ ] Wake only on CAN activity or scheduled TX intervals

## Priority 9: Sleep Mode Implementation (potential 80%+ savings when idle)
- [ ] Evaluate if system can enter STOP mode between CAN intervals
- [ ] Configure separate low-power timer for wake-up events
- [ ] Implement RTC or LPTIM-based wake scheduler
- [ ] Test current consumption in sleep vs active modes

## Priority 10: Memory Optimization
- [ ] Verify all large lookup tables (LUT_Z24, LUT_Z22) are in const/FLASH
- [ ] Audit other static arrays for FLASH placement
- [ ] Consider compressed lookup tables if memory-constrained

---

## Implementation Notes

### GPIO EXTI Setup (Priority 1)
All switch pins are on GPIOC:
- SW_AUTO (PC11), IN_BRAKE (PC12), SW_HAZARD (PC13), SW_HEADLIGHT (PC0)
- IN_SHELL_RELAY (PC1), SW_WIPER (PC6), SW_MC_OW (PC7), SW_LIGHTS (PC8)
- Need to handle rising/falling edges appropriately

### ADC Single-Shot (Priority 2)
Current flow: `HAL_ADC_Start() → PollForConversion(5ms) → Stop()`
New flow: `HAL_ADC_Start() → [interrupt fires] → read in callback`

### Throttle Math (Priority 4)
Replace:
```c
int32_t throttle_buffer = ((double)pot_value.current / 1023) * 100000;
```
With fixed-point (multiply then shift):
```c
int32_t throttle_buffer = (pot_value.current * 100000) / 1023;
```
Or precompute scaling factor to avoid division.

---

## Testing Strategy
1. Measure baseline current consumption
2. Implement Priority 1-3 changes
3. Retest current consumption (should see 30-50% improvement)
4. Implement remaining priorities incrementally
5. Profile power in different operational modes (idle, driving, wiper active)

## Current Measurements Needed
- [ ] Baseline power draw (all systems active)
- [ ] Power draw after Priority 1-3 fixes
- [ ] Power draw with all optimizations
- [ ] Power draw in sleep mode (if implemented)
