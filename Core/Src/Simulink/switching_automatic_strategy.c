/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: switching_automatic_strategy.c
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
#include "switching_automatic_strategy.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* Output and update for atomic system: '<Root>/Switching Automatic Strategy' */
void switching_automatic_strategy(real32_T rtu_SpeedInms, real32_T
  rtu_DistanceInm, real32_T rtu_Laptime, real32_T rtu_RPM, real32_T
  rtu_Totaldiff, real32_T *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T
  *rty_TorqueRef)
{
  real32_T rtb_SPDerror_o;
  real32_T rtb_uDLookupTable;

  /* Switch: '<S3>/Switch' incorporates:
   *  Constant: '<S3>/Constant1'
   *  Constant: '<S3>/Max RPM'
   *  RelationalOperator: '<S3>/LessThanOrEqual'
   */
  if (rtu_RPM <= 379.489746F) {
    /* Sum: '<S3>/Sum1' incorporates:
     *  Constant: '<S3>/Constant2'
     *  Lookup_n-D: '<S3>/n-D Lookup Table6'
     *  Product: '<S3>/Divide'
     *  Product: '<S3>/Product5'
     *  Sum: '<S3>/Sum6'
     */
    rtb_SPDerror_o = (107.0F - rtu_Totaldiff) / 107.0F * look1_iflf_linlcpw
      (rtu_Laptime, code_gen_main_ConstP.pooled1, code_gen_main_ConstP.pooled11,
       1070U) - rtu_SpeedInms;

    /* Outputs for Atomic SubSystem: '<S3>/Switching Control' */
    /* Switch: '<S58>/Switch4' incorporates:
     *  Constant: '<S58>/Constant'
     *  RelationalOperator: '<S58>/Relational Operator1'
     */
    if (rtb_SPDerror_o > 0.5F) {
      /* Switch: '<S58>/Switch' incorporates:
       *  Constant: '<S3>/Switching rpm'
       *  Constant: '<S58>/Constant1'
       *  Constant: '<S58>/Trq min'
       *  RelationalOperator: '<S58>/Relational Operator2'
       *  RelationalOperator: '<S58>/Relational Operator3'
       *  Switch: '<S58>/Switch1'
       */
      if (rtu_RPM < 221.936295F) {
        *rty_TorqueGain = -40.31F;
      } else if (rtu_RPM > 221.936295F) {
        /* Switch: '<S58>/Switch1' incorporates:
         *  Constant: '<S58>/Trq min limited'
         */
        *rty_TorqueGain = -13.39F;
      } else {
        *rty_TorqueGain = 0.0F;
      }

      /* End of Switch: '<S58>/Switch' */
    } else {
      boolean_T rtb_RelationalOperator4;

      /* RelationalOperator: '<S58>/Relational Operator4' incorporates:
       *  Constant: '<S58>/Constant2'
       */
      rtb_RelationalOperator4 = (rtb_SPDerror_o < -0.5F);

      /* Switch: '<S58>/Switch2' incorporates:
       *  Constant: '<S3>/Switching rpm'
       *  Constant: '<S58>/Constant3'
       *  Constant: '<S58>/Trq max1'
       *  Logic: '<S58>/AND2'
       *  Logic: '<S58>/AND3'
       *  RelationalOperator: '<S58>/Relational Operator2'
       *  RelationalOperator: '<S58>/Relational Operator3'
       *  Switch: '<S58>/Switch3'
       */
      if ((rtu_RPM < 221.936295F) && rtb_RelationalOperator4) {
        *rty_TorqueGain = 40.31F;
      } else if (rtb_RelationalOperator4 && (rtu_RPM > 221.936295F)) {
        /* Switch: '<S58>/Switch3' incorporates:
         *  Constant: '<S58>/Trq limited'
         */
        *rty_TorqueGain = 13.39F;
      } else {
        *rty_TorqueGain = 0.0F;
      }

      /* End of Switch: '<S58>/Switch2' */
    }

    /* End of Switch: '<S58>/Switch4' */
    /* End of Outputs for SubSystem: '<S3>/Switching Control' */

    /* Lookup_n-D: '<S3>/n-D Lookup Table8' */
    *rty_TorqueRef = look1_iflf_linlcpw(rtu_DistanceInm,
      code_gen_main_ConstP.pooled3, code_gen_main_ConstP.pooled2, 1070U);

    /* Sum: '<S3>/Sum2' */
    rtb_SPDerror_o = *rty_TorqueRef - *rty_TorqueGain;

    /* Lookup_n-D: '<S3>/1-D Lookup Table' */
    rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM, code_gen_main_ConstP.pooled5,
      code_gen_main_ConstP.pooled4, 49U);

    /* Switch: '<S3>/Switch1' incorporates:
     *  RelationalOperator: '<S3>/LessThanOrEqual1'
     */
    if (rtb_SPDerror_o <= rtb_uDLookupTable) {
      rtb_uDLookupTable = rtb_SPDerror_o;
    }

    /* End of Switch: '<S3>/Switch1' */
  } else {
    rtb_uDLookupTable = 0.0F;
  }

  /* End of Switch: '<S3>/Switch' */

  /* Sum: '<S3>/Sum4' incorporates:
   *  Constant: '<S3>/Constant3'
   *  Constant: '<S3>/Constant4'
   *  Constant: '<S3>/Constant5'
   *  Product: '<S3>/Product3'
   *  Product: '<S3>/Product4'
   */
  rtb_SPDerror_o = (rtu_RPM * 7.2E-5F + rtb_uDLookupTable * 0.02384F) +
    0.007204F;

  /* Saturate: '<S3>/Saturation' */
  if (rtb_SPDerror_o > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (rtb_SPDerror_o < 0.0F) {
    *rty_ThrottleOut = 0.0F;
  } else {
    *rty_ThrottleOut = rtb_SPDerror_o;
  }

  /* End of Saturate: '<S3>/Saturation' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
