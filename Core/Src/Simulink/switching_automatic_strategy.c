/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: switching_automatic_strategy.c
 *
 * Code generated for Simulink model 'auto_strat_C_generate'.
 *
 * Model version                  : 1.17
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu May 28 16:07:52 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "rtwtypes.h"
#include "switching_automatic_strategy.h"
#include "auto_strat_C_generate.h"
#include "auto_strat_C_generate_private.h"

/* Output and update for atomic system: '<Root>/Switching Automatic Strategy' */
void switching_automatic_strategy(real32_T rtu_SpeedInms, real32_T
  rtu_DistanceInm, real32_T rtu_Laptime, real32_T rtu_RPM, real32_T
  rtu_Totaldiff, real32_T *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T
  *rty_TorqueRef)
{
  real32_T rtb_SPDerror_o;
  real32_T rtb_uDLookupTable;

  /* Switch: '<S2>/Switch' incorporates:
   *  Constant: '<S2>/Constant1'
   *  Constant: '<S2>/Max RPM'
   *  RelationalOperator: '<S2>/LessThanOrEqual'
   */
  if (rtu_RPM <= 379.489746F) {
    /* Sum: '<S2>/Sum1' incorporates:
     *  Constant: '<S2>/Constant2'
     *  Lookup_n-D: '<S2>/n-D Lookup Table6'
     *  Product: '<S2>/Divide'
     *  Product: '<S2>/Product5'
     *  Sum: '<S2>/Sum6'
     */
    rtb_SPDerror_o = (107.0F - rtu_Totaldiff) / 107.0F * look1_iflf_linlcpw
      (rtu_Laptime, auto_strat_C_generate_ConstP.pooled1,
       auto_strat_C_generate_ConstP.pooled11, 1070U) - rtu_SpeedInms;

    /* Outputs for Atomic SubSystem: '<S2>/Switching Control' */
    /* Switch: '<S4>/Switch4' incorporates:
     *  Constant: '<S4>/Constant'
     *  RelationalOperator: '<S4>/Relational Operator1'
     */
    if (rtb_SPDerror_o > 0.5F) {
      /* Switch: '<S4>/Switch' incorporates:
       *  Constant: '<S2>/Switching rpm'
       *  Constant: '<S4>/Constant1'
       *  Constant: '<S4>/Trq min'
       *  RelationalOperator: '<S4>/Relational Operator2'
       *  RelationalOperator: '<S4>/Relational Operator3'
       *  Switch: '<S4>/Switch1'
       */
      if (rtu_RPM < 221.936295F) {
        *rty_TorqueGain = -40.31F;
      } else if (rtu_RPM > 221.936295F) {
        /* Switch: '<S4>/Switch1' incorporates:
         *  Constant: '<S4>/Trq min limited'
         */
        *rty_TorqueGain = -13.39F;
      } else {
        *rty_TorqueGain = 0.0F;
      }

      /* End of Switch: '<S4>/Switch' */
    } else {
      boolean_T rtb_RelationalOperator4;

      /* RelationalOperator: '<S4>/Relational Operator4' incorporates:
       *  Constant: '<S4>/Constant2'
       */
      rtb_RelationalOperator4 = (rtb_SPDerror_o < -0.5F);

      /* Switch: '<S4>/Switch2' incorporates:
       *  Constant: '<S2>/Switching rpm'
       *  Constant: '<S4>/Constant3'
       *  Constant: '<S4>/Trq max1'
       *  Logic: '<S4>/AND2'
       *  Logic: '<S4>/AND3'
       *  RelationalOperator: '<S4>/Relational Operator2'
       *  RelationalOperator: '<S4>/Relational Operator3'
       *  Switch: '<S4>/Switch3'
       */
      if ((rtu_RPM < 221.936295F) && rtb_RelationalOperator4) {
        *rty_TorqueGain = 40.31F;
      } else if (rtb_RelationalOperator4 && (rtu_RPM > 221.936295F)) {
        /* Switch: '<S4>/Switch3' incorporates:
         *  Constant: '<S4>/Trq limited'
         */
        *rty_TorqueGain = 13.39F;
      } else {
        *rty_TorqueGain = 0.0F;
      }

      /* End of Switch: '<S4>/Switch2' */
    }

    /* End of Switch: '<S4>/Switch4' */
    /* End of Outputs for SubSystem: '<S2>/Switching Control' */

    /* Lookup_n-D: '<S2>/n-D Lookup Table8' */
    *rty_TorqueRef = look1_iflf_linlcpw(rtu_DistanceInm,
      auto_strat_C_generate_ConstP.pooled3, auto_strat_C_generate_ConstP.pooled2,
      1070U);

    /* Sum: '<S2>/Sum2' */
    rtb_SPDerror_o = *rty_TorqueRef - *rty_TorqueGain;

    /* Lookup_n-D: '<S2>/1-D Lookup Table' */
    rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM,
      auto_strat_C_generate_ConstP.pooled5, auto_strat_C_generate_ConstP.pooled4,
      49U);

    /* Switch: '<S2>/Switch1' incorporates:
     *  RelationalOperator: '<S2>/LessThanOrEqual1'
     */
    if (rtb_SPDerror_o <= rtb_uDLookupTable) {
      rtb_uDLookupTable = rtb_SPDerror_o;
    }

    /* End of Switch: '<S2>/Switch1' */
  } else {
    rtb_uDLookupTable = 0.0F;
  }

  /* End of Switch: '<S2>/Switch' */

  /* Sum: '<S2>/Sum4' incorporates:
   *  Constant: '<S2>/Constant3'
   *  Constant: '<S2>/Constant4'
   *  Constant: '<S2>/Constant5'
   *  Product: '<S2>/Product3'
   *  Product: '<S2>/Product4'
   */
  rtb_SPDerror_o = (rtu_RPM * 7.2E-5F + rtb_uDLookupTable * 0.02384F) +
    0.007204F;

  /* Saturate: '<S2>/Saturation' */
  if (rtb_SPDerror_o > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (rtb_SPDerror_o < 0.0F) {
    *rty_ThrottleOut = 0.0F;
  } else {
    *rty_ThrottleOut = rtb_SPDerror_o;
  }

  /* End of Saturate: '<S2>/Saturation' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
