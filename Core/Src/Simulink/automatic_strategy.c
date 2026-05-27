/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: automatic_strategy.c
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

#include "rtwtypes.h"
#include "automatic_strategy.h"
#include "Automatic.h"
#include "Automatic_private.h"

/* Output and update for atomic system: '<Root>/Automatic Strategy Subsystem' */
void automatic_strategy(real32_T rtu_SpeedInms, real32_T rtu_DistanceInm,
  real32_T rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Previouslaptime, real32_T
  *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T *rty_TorqueRef,
  DW_automatic_strategy_T *localDW)
{
  real32_T rtb_Product1;
  real32_T rtb_Product5;

  /* Lookup_n-D: '<S1>/n-D Lookup Table8' */
  *rty_TorqueRef = look1_iflf_linlcpw(rtu_DistanceInm,
    Automatic_ConstP.nDLookupTable8_bp01Data,
    Automatic_ConstP.nDLookupTable8_tableData, 1070U);

  /* Lookup_n-D: '<S1>/n-D Lookup Table6' */
  *rty_TorqueGain = look1_iflf_linlcpw(rtu_Laptime, Automatic_ConstP.pooled1,
    Automatic_ConstP.nDLookupTable6_tableData, 1070U);

  /* Product: '<S1>/Product5' incorporates:
   *  Constant: '<S1>/Constant6'
   *  Product: '<S1>/Divide'
   *  Sum: '<S1>/Sum5'
   *  Sum: '<S1>/Sum6'
   */
  rtb_Product5 = (107.0F - (107.0F - rtu_Previouslaptime)) / 107.0F *
    *rty_TorqueGain;

  /* Lookup_n-D: '<S1>/n-D Lookup Table5' */
  *rty_TorqueGain = look1_iflf_linlcpw(rtu_Laptime, Automatic_ConstP.pooled1,
    Automatic_ConstP.nDLookupTable5_tableData, 1070U);

  /* Product: '<S1>/Product1' incorporates:
   *  DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
   *  Sum: '<S1>/Sum3'
   */
  rtb_Product1 = (localDW->DiscreteTimeIntegrator_DSTATE - rtu_DistanceInm) *
    *rty_TorqueGain;

  /* Lookup_n-D: '<S1>/n-D Lookup Table3' */
  *rty_TorqueGain = look1_iflf_linlcpw(rtu_Laptime, Automatic_ConstP.pooled1,
    Automatic_ConstP.nDLookupTable3_tableData, 1070U);

  /* Sum: '<S1>/Sum' incorporates:
   *  Constant: '<S1>/Wind Disturbance Estimation'
   *  Lookup_n-D: '<S1>/n-D Lookup Table4'
   *  Product: '<S1>/Product'
   *  Product: '<S1>/Product2'
   *  Sum: '<S1>/Sum1'
   */
  *rty_TorqueGain = ((rtb_Product5 - rtu_SpeedInms) * look1_iflf_linlcpw
                     (rtu_Laptime, Automatic_ConstP.pooled1,
                      Automatic_ConstP.nDLookupTable4_tableData, 1070U) +
                     rtb_Product1) + *rty_TorqueGain * 0.0F;

  /* Switch: '<S1>/Switch' incorporates:
   *  Constant: '<S1>/Constant1'
   *  Constant: '<S1>/Max RPM'
   *  RelationalOperator: '<S1>/LessThanOrEqual'
   */
  if (rtu_RPM <= 379.489746F) {
    real32_T rtb_uDLookupTable;

    /* Sum: '<S1>/Sum2' */
    rtb_Product1 = *rty_TorqueRef - *rty_TorqueGain;

    /* Lookup_n-D: '<S1>/1-D Lookup Table' */
    rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM,
      Automatic_ConstP.uDLookupTable_bp01Data,
      Automatic_ConstP.uDLookupTable_tableData, 49U);

    /* Switch: '<S1>/Switch1' incorporates:
     *  RelationalOperator: '<S1>/LessThanOrEqual1'
     */
    if (rtb_Product1 <= rtb_uDLookupTable) {
      *rty_ThrottleOut = rtb_Product1;
    } else {
      *rty_ThrottleOut = rtb_uDLookupTable;
    }

    /* End of Switch: '<S1>/Switch1' */
  } else {
    *rty_ThrottleOut = 0.0F;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Sum: '<S1>/Sum4' incorporates:
   *  Constant: '<S1>/Constant3'
   *  Constant: '<S1>/Constant4'
   *  Constant: '<S1>/Constant5'
   *  Product: '<S1>/Product3'
   *  Product: '<S1>/Product4'
   */
  *rty_ThrottleOut = (rtu_RPM * 7.2E-5F + *rty_ThrottleOut * 0.02384F) +
    0.007204F;

  /* Saturate: '<S1>/Saturation' */
  if (*rty_ThrottleOut > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (*rty_ThrottleOut < 0.0F) {
    *rty_ThrottleOut = 0.0F;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_DSTATE += 0.05F * rtb_Product5;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
