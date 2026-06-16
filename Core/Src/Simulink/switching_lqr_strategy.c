/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: switching_lqr_strategy.c
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
#include "switching_lqr_strategy.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* Output and update for atomic system: '<Root>/Switching LQR Strategy' */
void switching_lqr_strategy(real32_T rtu_Distance, real32_T rtu_RPM, real32_T
  rtu_Total_Diff, real32_T *rty_Torque_Ref_Out, real32_T *rty_Torque_Gain,
  real32_T *rty_Torque_Base)
{
  real32_T rtb_uDLookupTable;

  /* Sum: '<S3>/Sum6' incorporates:
   *  Constant: '<S3>/Constant2'
   */
  *rty_Torque_Gain = rtu_Total_Diff + 107.0F;

  /* Product: '<S3>/Divide' incorporates:
   *  Constant: '<S3>/Constant2'
   */
  *rty_Torque_Gain /= 107.0F;

  /* Switch: '<S3>/Switch2' incorporates:
   *  Constant: '<S3>/Constant'
   *  Constant: '<S3>/Constant9'
   *  Lookup_n-D: '<S3>/slow'
   *  RelationalOperator: '<S3>/Relational Operator'
   *  RelationalOperator: '<S3>/Relational Operator3'
   *  Switch: '<S3>/Switch5'
   */
  if (*rty_Torque_Gain <= 0.99F) {
    *rty_Torque_Base = code_gen_main_ConstP.slow_tableData[plook_u32f_linckan
      (rtu_Distance, code_gen_main_ConstP.pooled13, 1070U)];
  } else if (*rty_Torque_Gain >= 1.01F) {
    /* Switch: '<S3>/Switch5' incorporates:
     *  Lookup_n-D: '<S3>/fast'
     */
    *rty_Torque_Base = code_gen_main_ConstP.fast_tableData[plook_u32f_linckan
      (rtu_Distance, code_gen_main_ConstP.pooled13, 1070U)];
  } else {
    /* Switch: '<S3>/Switch5' incorporates:
     *  Lookup_n-D: '<S3>/norm'
     */
    *rty_Torque_Base = code_gen_main_ConstP.pooled12[plook_u32f_linckan
      (rtu_Distance, code_gen_main_ConstP.pooled13, 1070U)];
  }

  /* End of Switch: '<S3>/Switch2' */

  /* Switch: '<S3>/Switch4' incorporates:
   *  Constant: '<S3>/Constant6'
   *  Constant: '<S3>/Constant7'
   *  Constant: '<S3>/Constant8'
   *  Constant: '<S3>/Max RPM'
   *  RelationalOperator: '<S3>/LessThanOrEqual'
   *  RelationalOperator: '<S3>/Relational Operator1'
   *  RelationalOperator: '<S3>/Relational Operator2'
   *  Switch: '<S3>/Switch'
   *  Switch: '<S3>/Switch3'
   */
  if (*rty_Torque_Base <= 5.0F) {
    rtb_uDLookupTable = 0.0F;
  } else if (*rty_Torque_Base >= 38.0F) {
    /* Switch: '<S3>/Switch3' incorporates:
     *  Constant: '<S3>/Max Load signal'
     */
    rtb_uDLookupTable = 1.0F;
  } else {
    if (rtu_RPM <= 379.489746F) {
      /* Lookup_n-D: '<S3>/1-D Lookup Table' incorporates:
       *  Switch: '<S3>/Switch'
       *  Switch: '<S3>/Switch3'
       */
      rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM,
        code_gen_main_ConstP.pooled3, code_gen_main_ConstP.pooled2, 49U);

      /* Switch: '<S3>/Switch1' incorporates:
       *  RelationalOperator: '<S3>/LessThanOrEqual1'
       *  Switch: '<S3>/Switch'
       *  Switch: '<S3>/Switch3'
       */
      if (*rty_Torque_Base <= rtb_uDLookupTable) {
        rtb_uDLookupTable = *rty_Torque_Base;
      }

      /* End of Switch: '<S3>/Switch1' */
    } else {
      /* Switch: '<S3>/Switch' incorporates:
       *  Constant: '<S3>/Constant1'
       *  Switch: '<S3>/Switch3'
       */
      rtb_uDLookupTable = 0.0F;
    }

    /* Switch: '<S3>/Switch3' incorporates:
     *  Constant: '<S3>/Constant3'
     *  Constant: '<S3>/Constant4'
     *  Constant: '<S3>/Constant5'
     *  Product: '<S3>/Product3'
     *  Product: '<S3>/Product4'
     *  Sum: '<S3>/Sum4'
     */
    rtb_uDLookupTable = (rtu_RPM * 7.2E-5F + rtb_uDLookupTable * 0.02384F) +
      0.007204F;
  }

  /* End of Switch: '<S3>/Switch4' */

  /* Saturate: '<S3>/Saturation' */
  if (rtb_uDLookupTable > 1.0F) {
    *rty_Torque_Ref_Out = 1.0F;
  } else if (rtb_uDLookupTable < 0.0F) {
    *rty_Torque_Ref_Out = 0.0F;
  } else {
    *rty_Torque_Ref_Out = rtb_uDLookupTable;
  }

  /* End of Saturate: '<S3>/Saturation' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
