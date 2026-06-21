/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: code_gen_main_private.h
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.31
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu Jun 18 15:23:46 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef code_gen_main_private_h_
#define code_gen_main_private_h_
#include "rtwtypes.h"
#include "code_gen_main_types.h"
#include "code_gen_main.h"

extern real32_T rt_modf_snf(real32_T u0, real32_T u1);
extern real32_T look1_iflf_linlcpw(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex);
extern real32_T look1_iflf_binlxpw(real32_T u0, const real32_T bp0[], const
  real32_T table[], uint32_T maxIndex);
extern uint32_T plook_u32f_linckan(real32_T u, const real32_T bp[], uint32_T
  maxIndex);
extern uint32_T linsearch_u32f(real32_T u, const real32_T bp[], uint32_T
  startIndex);

#endif                                 /* code_gen_main_private_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
