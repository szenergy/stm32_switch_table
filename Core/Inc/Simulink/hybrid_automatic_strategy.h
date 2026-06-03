/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: hybrid_automatic_strategy.h
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

#ifndef hybrid_automatic_strategy_h_
#define hybrid_automatic_strategy_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rt_nonfinite.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

/* Block states (default storage) for system '<Root>/Hybrid' */
typedef struct {
  real32_T DelayInput1_DSTATE;         /* '<S6>/Delay Input1' */
  real32_T DiscreteTimeIntegrator_DSTATE;/* '<S2>/Discrete-Time Integrator' */
  int8_T DiscreteTimeIntegrator_PrevRese;/* '<S2>/Discrete-Time Integrator' */
} DW_hybrid_automatic_strategy_T;

extern void hybrid_automatic_strategy_Init(DW_hybrid_automatic_strategy_T
  *localDW);
extern void hybrid_automatic_strategy(real32_T rtu_SpeedInms, real32_T
  rtu_DistanceInm, real32_T rtu_Laptime, real32_T rtu_RPM, real32_T
  rtu_Totaldiff, real32_T *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T
  *rty_TorqueRef, DW_hybrid_automatic_strategy_T *localDW);

#endif                                 /* hybrid_automatic_strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
