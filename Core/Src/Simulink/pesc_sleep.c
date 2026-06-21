/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: pesc_sleep.c
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.33
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Mon Jun 22 00:25:26 2026
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
boolean_T pesc_sleep(real32_T rtu_Distance, real32_T rtu_Speed)
{
  /* RelationalOperator: '<S2>/GreaterThan' incorporates:
   *  Constant: '<S2>/Seconds before acceleration'
   *  Lookup_n-D: '<S2>/Sleep lookup'
   *  Product: '<S2>/Distance traveled'
   */
  return code_gen_main_ConstP.Sleeplookup_tableData[plook_u32f_linckan
    (rtu_Distance, code_gen_main_ConstP.pooled3, 1910U)] > rtu_Speed * 4.0F;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
