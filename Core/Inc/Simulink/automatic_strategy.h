/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: automatic_strategy.h
 *
 * Code generated for Simulink model 'auto_strat_C_generate'.
 *
 * Model version                  : 1.17
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu May 28 16:07:52 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef automatic_strategy_h_
#define automatic_strategy_h_
#ifndef auto_strat_C_generate_COMMON_INCLUDES_
#define auto_strat_C_generate_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                              /* auto_strat_C_generate_COMMON_INCLUDES_ */

/* Block states (default storage) for system '<Root>/Automatic Strategy' */
typedef struct {
  real32_T DelayInput1_DSTATE;         /* '<S3>/Delay Input1' */
  real32_T DiscreteTimeIntegrator_DSTATE;/* '<S1>/Discrete-Time Integrator' */
  int8_T DiscreteTimeIntegrator_PrevRese;/* '<S1>/Discrete-Time Integrator' */
} DW_automatic_strategy_T;

extern void automatic_strategy_Init(DW_automatic_strategy_T *localDW);
extern void automatic_strategy(real32_T rtu_SpeedInms, real32_T rtu_DistanceInm,
  real32_T rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Totaldiff, real32_T
  *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T *rty_TorqueRef,
  DW_automatic_strategy_T *localDW);

#endif                                 /* automatic_strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
