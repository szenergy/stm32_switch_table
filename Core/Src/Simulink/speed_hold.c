/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: speed_hold.c
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
#include "speed_hold.h"

/* Output and update for atomic system: '<Root>/Speed Hold' */
void speed_hold(real32_T rtu_CurrentSpeed, real32_T rtu_DesiredSpeed, real32_T
                *rty_ThrottleOut, real32_T *rty_Error, DW_speed_hold_T *localDW)
{
  real32_T rtb_FilterCoefficient;
  real32_T rtb_IntegralGain;
  real32_T rtb_Sum;
  real32_T rtb_Switch_p;
  int8_T tmp;
  int8_T tmp_0;

  /* Sum: '<S3>/Calculate Error' */
  *rty_Error = rtu_DesiredSpeed - rtu_CurrentSpeed;

  /* Gain: '<S46>/Proportional Gain' */
  rtb_Sum = 0.025F * *rty_Error;

  /* Gain: '<S34>/Derivative Gain' incorporates:
   *  Gain: '<S38>/Integral Gain'
   */
  rtb_Switch_p = 0.01F * *rty_Error;

  /* Gain: '<S44>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S36>/Filter'
   *  Gain: '<S34>/Derivative Gain'
   *  Sum: '<S36>/SumD'
   */
  rtb_FilterCoefficient = (rtb_Switch_p - localDW->Filter_DSTATE) * 100.0F;

  /* Sum: '<S51>/Sum Fdbk' */
  rtb_IntegralGain = (rtb_Sum + localDW->Integrator_DSTATE) +
    rtb_FilterCoefficient;

  /* DeadZone: '<S33>/DeadZone' */
  if (rtb_IntegralGain > 1.0F) {
    rtb_IntegralGain--;
  } else if (rtb_IntegralGain >= -1.0F) {
    rtb_IntegralGain = 0.0F;
  } else {
    rtb_IntegralGain++;
  }

  /* End of DeadZone: '<S33>/DeadZone' */

  /* Switch: '<S31>/Switch1' incorporates:
   *  Constant: '<S31>/Clamping_zero'
   *  Constant: '<S31>/Constant'
   *  Constant: '<S31>/Constant2'
   *  RelationalOperator: '<S31>/fix for DT propagation issue'
   */
  if (rtb_IntegralGain > 0.0F) {
    tmp = 1;
  } else {
    tmp = -1;
  }

  /* Switch: '<S31>/Switch2' incorporates:
   *  Constant: '<S31>/Clamping_zero'
   *  Constant: '<S31>/Constant3'
   *  Constant: '<S31>/Constant4'
   *  RelationalOperator: '<S31>/fix for DT propagation issue1'
   */
  if (rtb_Switch_p > 0.0F) {
    tmp_0 = 1;
  } else {
    tmp_0 = -1;
  }

  /* Switch: '<S31>/Switch' incorporates:
   *  Constant: '<S31>/Clamping_zero'
   *  Constant: '<S31>/Constant1'
   *  Logic: '<S31>/AND3'
   *  RelationalOperator: '<S31>/Equal1'
   *  RelationalOperator: '<S31>/Relational Operator'
   *  Switch: '<S31>/Switch1'
   *  Switch: '<S31>/Switch2'
   */
  if ((rtb_IntegralGain != 0.0F) && (tmp == tmp_0)) {
    rtb_Switch_p = 0.0F;
  }

  /* End of Switch: '<S31>/Switch' */

  /* DiscreteIntegrator: '<S41>/Integrator' */
  rtb_IntegralGain = 0.025F * rtb_Switch_p;

  /* DiscreteIntegrator: '<S41>/Integrator' */
  rtb_Switch_p = rtb_IntegralGain + localDW->Integrator_DSTATE;

  /* Sum: '<S50>/Sum' */
  rtb_Sum = (rtb_Sum + rtb_Switch_p) + rtb_FilterCoefficient;

  /* Saturate: '<S48>/Saturation' */
  if (rtb_Sum > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (rtb_Sum < -1.0F) {
    *rty_ThrottleOut = -1.0F;
  } else {
    *rty_ThrottleOut = rtb_Sum;
  }

  /* End of Saturate: '<S48>/Saturation' */

  /* Update for DiscreteIntegrator: '<S36>/Filter' */
  localDW->Filter_DSTATE += 0.05F * rtb_FilterCoefficient;

  /* Update for DiscreteIntegrator: '<S41>/Integrator' */
  localDW->Integrator_DSTATE = rtb_IntegralGain + rtb_Switch_p;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
