/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: speed_hold.c
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.19
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Tue Jun  2 09:58:58 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "rtwtypes.h"
#include "speed_hold.h"

/* Output and update for atomic system: '<Root>/Speed Hold' */
void speed_hold(real32_T rtu_CurrentSpeed, real32_T rtu_DesiredSpeed, real32_T
                *rty_ThrottleOut, real32_T *rty_Error, DW_speed_hold_T *localDW)
{
  real32_T rtb_FilterCoefficient;
  real32_T u0;

  /* Sum: '<S2>/Calculate Error' */
  *rty_Error = rtu_CurrentSpeed - rtu_DesiredSpeed;

  /* Gain: '<S43>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S35>/Filter'
   *  Gain: '<S33>/Derivative Gain'
   *  Sum: '<S35>/SumD'
   */
  rtb_FilterCoefficient = (0.0F * *rty_Error - localDW->Filter_DSTATE) * 100.0F;

  /* Sum: '<S49>/Sum' incorporates:
   *  DiscreteIntegrator: '<S40>/Integrator'
   *  Gain: '<S45>/Proportional Gain'
   */
  u0 = (0.25F * *rty_Error + localDW->Integrator_DSTATE) + rtb_FilterCoefficient;

  /* Saturate: '<S47>/Saturation' */
  if (u0 > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (u0 < -0.6F) {
    *rty_ThrottleOut = -0.6F;
  } else {
    *rty_ThrottleOut = u0;
  }

  /* End of Saturate: '<S47>/Saturation' */

  /* Update for DiscreteIntegrator: '<S35>/Filter' */
  localDW->Filter_DSTATE += 0.05F * rtb_FilterCoefficient;

  /* Update for DiscreteIntegrator: '<S40>/Integrator' incorporates:
   *  Gain: '<S37>/Integral Gain'
   */
  localDW->Integrator_DSTATE += 0.1F * *rty_Error * 0.05F;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
