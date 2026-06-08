/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: ltv_lqr_strategy.h
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.25
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Jun  5 09:51:16 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef ltv_lqr_strategy_h_
#define ltv_lqr_strategy_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

/* Block states (default storage) for system '<Root>/LTV - LQR Strategy' */
typedef struct {
  real32_T DiscreteTimeIntegrator_DSTATE;/* '<S1>/Discrete-Time Integrator' */
  int8_T DiscreteTimeIntegrator_PrevRese;/* '<S1>/Discrete-Time Integrator' */
} DW_ltv_lqr_strategy_T;

extern void ltv_lqr_strategy_Init(DW_ltv_lqr_strategy_T *localDW);
extern void ltv_lqr_strategy(real32_T rtu_Speed, real32_T rtu_Distance, real32_T
  rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Total_Diff, real32_T
  *rty_Torque_Ref_Out, real32_T *rty_Torque_Gain, real32_T *rty_Torque_Base,
  real32_T *rty_Speed_Ref, real32_T *rty_Distance_Ref, DW_ltv_lqr_strategy_T
  *localDW);

#endif                                 /* ltv_lqr_strategy_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
