/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: speed_hold.h
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.21
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Tue Jun  2 21:25:43 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef speed_hold_h_
#define speed_hold_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

/* Block states (default storage) for system '<Root>/Speed Hold' */
typedef struct {
  real32_T Filter_DSTATE;              /* '<S38>/Filter' */
  real32_T Integrator_DSTATE;          /* '<S43>/Integrator' */
} DW_speed_hold_T;

extern void speed_hold(real32_T rtu_CurrentSpeed, real32_T rtu_DesiredSpeed,
  real32_T *rty_ThrottleOut, real32_T *rty_Error, DW_speed_hold_T *localDW);

#endif                                 /* speed_hold_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
