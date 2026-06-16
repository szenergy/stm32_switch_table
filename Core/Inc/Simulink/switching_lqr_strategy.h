/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: switching_lqr_strategy.h
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.29
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Mon Jun 15 20:42:22 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef switching_lqr_strategy_h_
#define switching_lqr_strategy_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

extern void switching_lqr_strategy(real32_T rtu_Distance, real32_T rtu_RPM,
  real32_T rtu_Total_Diff, real32_T *rty_Torque_Ref_Out, real32_T
  *rty_Torque_Gain, real32_T *rty_Torque_Base);

#endif                                 /* switching_lqr_strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
