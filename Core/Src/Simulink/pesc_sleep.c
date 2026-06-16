/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: pesc_sleep.c
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

#include "rtwtypes.h"
#include "pesc_sleep.h"
#include "code_gen_main_private.h"
#include "code_gen_main.h"
#include "rt_nonfinite.h"
#include <math.h>
#include <float.h>

real32_T rt_modf_snf(real32_T u0, real32_T u1)
{
  real32_T y;
  y = u0;
  if (u1 == 0.0F) {
    if (u0 == 0.0F) {
      y = u1;
    }
  } else if (rtIsNaNF(u0) || rtIsNaNF(u1) || rtIsInfF(u0)) {
    y = (rtNaNF);
  } else if (u0 == 0.0F) {
    y = 0.0F / u1;
  } else if (rtIsInfF(u1)) {
    if ((u1 < 0.0F) != (u0 < 0.0F)) {
      y = u1;
    }
  } else {
    boolean_T yEq;
    y = fmodf(u0, u1);
    yEq = (y == 0.0F);
    if ((!yEq) && (u1 > floorf(u1))) {
      real32_T q;
      q = fabsf(u0 / u1);
      yEq = !(fabsf(q - floorf(q + 0.5F)) > FLT_EPSILON * q);
    }

    if (yEq) {
      y = u1 * 0.0F;
    } else if ((u0 < 0.0F) != (u1 < 0.0F)) {
      y += u1;
    }
  }

  return y;
}

/* Output and update for atomic system: '<Root>/PESC Sleep' */
boolean_T pesc_sleep(real32_T rtu_Distance, real32_T rtu_Speed)
{
  boolean_T rty_PESC_Sleep_0;

  /* Logic: '<S2>/Logical Operator' incorporates:
   *  Constant: '<S2>/Max distance'
   *  Constant: '<S2>/Min Torque to wake PESC'
   *  Constant: '<S2>/Seconds to wake before acceleration point'
   *  Logic: '<S2>/Logical Operator1'
   *  Lookup_n-D: '<S2>/1D Torque from Distance'
   *  Lookup_n-D: '<S2>/1D Torque from Distance1'
   *  Math: '<S2>/Math Function'
   *  Product: '<S2>/Product'
   *  RelationalOperator: '<S2>/GreaterThan'
   *  RelationalOperator: '<S2>/GreaterThan1'
   *  Sum: '<S2>/Sum'
   */
  rty_PESC_Sleep_0 = ((!(code_gen_main_ConstP.pooled12[plook_u32f_linckan
    (rtu_Distance, code_gen_main_ConstP.pooled13, 1070U)] > 1.0F)) &&
                      (!(code_gen_main_ConstP.pooled12[plook_u32f_linckan
    (rt_modf_snf(rtu_Speed * 3.0F + rtu_Distance, 750.0F),
     code_gen_main_ConstP.pooled13, 1070U)] > 1.0F)));
  return rty_PESC_Sleep_0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
