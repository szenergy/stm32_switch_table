/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: ltv_lqr_strategy.c
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

#include "ltv_lqr_strategy.h"
#include "rtwtypes.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* System initialize for atomic system: '<Root>/LTV - LQR Strategy' */
void ltv_lqr_strategy_Init(DW_ltv_lqr_strategy_T *localDW)
{
  /* InitializeConditions for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_PrevRese = 2;
}

/* Output and update for atomic system: '<Root>/LTV - LQR Strategy' */
void ltv_lqr_strategy(real32_T rtu_Speed, real32_T rtu_Distance, real32_T
                      rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Total_Diff,
                      real32_T *rty_Torque_Ref_Out, real32_T *rty_Torque_Gain,
                      real32_T *rty_Torque_Base, real32_T *rty_Speed_Ref,
                      real32_T *rty_Distance_Ref, DW_ltv_lqr_strategy_T *localDW)
{
  real32_T rtb_Switch4;
  boolean_T rtb_Equal;

  /* Lookup_n-D: '<S1>/1D Speed from laptime' */
  *rty_Speed_Ref = look1_iflf_linlcpw(rtu_Laptime, code_gen_main_ConstP.pooled1,
    code_gen_main_ConstP.uDSpeedfromlaptime_tableData, 1910U);

  /* Sum: '<S1>/Sum6' incorporates:
   *  Constant: '<S1>/Optimal lap'
   */
  *rty_Distance_Ref = rtu_Total_Diff + 191.1F;

  /* Product: '<S1>/Product5' incorporates:
   *  Constant: '<S1>/Optimal lap'
   *  Product: '<S1>/Divide'
   */
  *rty_Speed_Ref *= *rty_Distance_Ref / 191.1F;

  /* RelationalOperator: '<S1>/Equal' incorporates:
   *  Constant: '<S1>/Constant9'
   */
  rtb_Equal = (rtu_Laptime == 0.0F);

  /* DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  if (rtb_Equal && (localDW->DiscreteTimeIntegrator_PrevRese <= 0)) {
    localDW->DiscreteTimeIntegrator_DSTATE = 0.0F;
  }

  *rty_Distance_Ref = localDW->DiscreteTimeIntegrator_DSTATE;

  /* End of DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */

  /* Lookup_n-D: '<S1>/1D Torque from Distance' */
  *rty_Torque_Base =
    code_gen_main_ConstP.uDTorquefromDistance_tableData[plook_u32f_linckan
    (rtu_Distance, code_gen_main_ConstP.pooled3, 1910U)];

  /* Switch: '<S1>/Switch2' incorporates:
   *  Constant: '<S1>/Constant'
   *  RelationalOperator: '<S1>/Relational Operator'
   */
  if (*rty_Torque_Base > 0.0F) {
    /* Sum: '<S1>/Sum' incorporates:
     *  Constant: '<S1>/Wind Disturbance Estimation'
     *  Lookup_n-D: '<S1>/1D K1 wff lookup'
     *  Lookup_n-D: '<S1>/1D K1 x1 lookup'
     *  Lookup_n-D: '<S1>/1D K1 x2 lookup'
     *  Product: '<S1>/Product'
     *  Product: '<S1>/Product1'
     *  Product: '<S1>/Product2'
     *  Sum: '<S1>/Sum1'
     *  Sum: '<S1>/Sum3'
     */
    *rty_Torque_Gain = ((*rty_Speed_Ref - rtu_Speed) * look1_iflf_linlcpw
                        (rtu_Laptime, code_gen_main_ConstP.pooled1,
                         code_gen_main_ConstP.uDK1x1lookup_tableData, 1910U) + (*
      rty_Distance_Ref - rtu_Distance) * look1_iflf_linlcpw(rtu_Laptime,
      code_gen_main_ConstP.pooled1, code_gen_main_ConstP.uDK1x2lookup_tableData,
      1910U)) + look1_iflf_linlcpw(rtu_Laptime, code_gen_main_ConstP.pooled1,
      code_gen_main_ConstP.uDK1wfflookup_tableData, 1910U) * 0.0F;
    rtb_Switch4 = *rty_Torque_Gain;
  } else {
    rtb_Switch4 = 0.0F;
  }

  /* End of Switch: '<S1>/Switch2' */

  /* Sum: '<S1>/Sum2' */
  rtb_Switch4 = *rty_Torque_Base - rtb_Switch4;

  /* Switch: '<S1>/Switch4' incorporates:
   *  Constant: '<S1>/Constant6'
   *  Constant: '<S1>/Constant7'
   *  Constant: '<S1>/Constant8'
   *  Constant: '<S1>/Max RPM'
   *  RelationalOperator: '<S1>/LessThanOrEqual'
   *  RelationalOperator: '<S1>/Relational Operator1'
   *  RelationalOperator: '<S1>/Relational Operator2'
   *  Switch: '<S1>/Switch'
   *  Switch: '<S1>/Switch3'
   */
  if (rtb_Switch4 <= 5.0F) {
    rtb_Switch4 = 0.0F;
  } else if (rtb_Switch4 >= 38.0F) {
    /* Switch: '<S1>/Switch3' incorporates:
     *  Constant: '<S1>/Max Load signal'
     */
    rtb_Switch4 = 1.0F;
  } else {
    real32_T rtb_uDPESCLimTorque;
    if (rtu_RPM <= 379.489746F) {
      /* Lookup_n-D: '<S1>/1D PESC Lim Torque' incorporates:
       *  Switch: '<S1>/Switch'
       *  Switch: '<S1>/Switch3'
       */
      rtb_uDPESCLimTorque = look1_iflf_binlxpw(rtu_RPM,
        code_gen_main_ConstP.uDPESCLimTorque_bp01Data,
        code_gen_main_ConstP.uDPESCLimTorque_tableData, 49U);

      /* Switch: '<S1>/Switch1' incorporates:
       *  RelationalOperator: '<S1>/LessThanOrEqual1'
       *  Switch: '<S1>/Switch'
       *  Switch: '<S1>/Switch3'
       */
      if (rtb_Switch4 <= rtb_uDPESCLimTorque) {
        rtb_uDPESCLimTorque = rtb_Switch4;
      }

      /* End of Switch: '<S1>/Switch1' */
    } else {
      /* Switch: '<S1>/Switch' incorporates:
       *  Constant: '<S1>/Constant1'
       *  Switch: '<S1>/Switch3'
       */
      rtb_uDPESCLimTorque = 0.0F;
    }

    /* Switch: '<S1>/Switch3' incorporates:
     *  Constant: '<S1>/Constant3'
     *  Constant: '<S1>/Constant4'
     *  Constant: '<S1>/Constant5'
     *  Product: '<S1>/Product3'
     *  Product: '<S1>/Product4'
     *  Sum: '<S1>/Sum4'
     */
    rtb_Switch4 = (rtu_RPM * 7.2E-5F + rtb_uDPESCLimTorque * 0.02384F) +
      0.007204F;
  }

  /* End of Switch: '<S1>/Switch4' */

  /* Saturate: '<S1>/Saturation' */
  if (rtb_Switch4 > 1.0F) {
    *rty_Torque_Ref_Out = 1.0F;
  } else if (rtb_Switch4 < 0.0F) {
    *rty_Torque_Ref_Out = 0.0F;
  } else {
    *rty_Torque_Ref_Out = rtb_Switch4;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_DSTATE += 0.05F * *rty_Speed_Ref;
  localDW->DiscreteTimeIntegrator_PrevRese = (int8_T)rtb_Equal;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
