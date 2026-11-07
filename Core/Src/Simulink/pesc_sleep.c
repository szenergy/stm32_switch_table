/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: pesc_sleep.c
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.37
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Mon Oct 12 14:37:01 2026
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
    (rtu_Distance, code_gen_main_ConstP.pooled5, 1910U)];
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
