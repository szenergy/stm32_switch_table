/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: pesc_sleep.c
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.35
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Mon Jun 22 17:55:40 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "rtwtypes.h"
#include "pesc_sleep.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* Output and update for atomic system: '<Root>/PESC Sleep' */
real32_T pesc_sleep(real32_T rtu_Distance)
{
  /* Lookup_n-D: '<S2>/PESC_sleep_LUT' */
  return code_gen_main_ConstP.PESC_sleep_LUT_tableData[plook_u32f_linckan
    (rtu_Distance, code_gen_main_ConstP.pooled3, 1910U)];
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
