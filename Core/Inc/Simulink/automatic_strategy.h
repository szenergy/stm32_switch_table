/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: automatic_strategy.h
 *
 * Code generated for Simulink model 'Automatic'.
 *
 * Model version                  : 1.6
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Wed May 27 11:02:37 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef automatic_strategy_h_
#define automatic_strategy_h_
#ifndef Automatic_COMMON_INCLUDES_
#define Automatic_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* Automatic_COMMON_INCLUDES_ */

/* Block states (default storage) for system '<Root>/Automatic Strategy Subsystem' */
typedef struct {
  real32_T DiscreteTimeIntegrator_DSTATE;/* '<S1>/Discrete-Time Integrator' */
} DW_automatic_strategy_T;

extern void automatic_strategy(real32_T rtu_SpeedInms, real32_T rtu_DistanceInm,
  real32_T rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Previouslaptime, real32_T
  *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T *rty_TorqueRef,
  DW_automatic_strategy_T *localDW);

#endif                                 /* automatic_strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
