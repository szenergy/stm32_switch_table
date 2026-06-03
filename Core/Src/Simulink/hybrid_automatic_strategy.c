/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: hybrid_automatic_strategy.c
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.21
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Tue Jun  2 21:25:43 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "hybrid_automatic_strategy.h"
#include "rtwtypes.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* System initialize for atomic system: '<Root>/Hybrid' */
void hybrid_automatic_strategy_Init(DW_hybrid_automatic_strategy_T *localDW)
{
  /* InitializeConditions for DiscreteIntegrator: '<S2>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_PrevRese = 2;
}

/* Output and update for atomic system: '<Root>/Hybrid' */
void hybrid_automatic_strategy(real32_T rtu_SpeedInms, real32_T rtu_DistanceInm,
  real32_T rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Totaldiff, real32_T
  *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T *rty_TorqueRef,
  DW_hybrid_automatic_strategy_T *localDW)
{
  real32_T rtb_Product5;
  real32_T rtb_SPDerror_n;
  real32_T rtb_Sum2;
  real32_T rtb_Switch;
  real32_T rtb_uDLookupTable;
  boolean_T rtb_RelationalOperator1;

  /* Product: '<S2>/Product5' incorporates:
   *  Constant: '<S2>/Constant2'
   *  Lookup_n-D: '<S2>/n-D Lookup Table6'
   *  Product: '<S2>/Divide'
   *  Sum: '<S2>/Sum6'
   */
  rtb_Product5 = (rtu_Totaldiff + 107.0F) / 107.0F * look1_iflf_linlcpw
    (rtu_Laptime, code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled14,
     1070U);

  /* Sum: '<S2>/Sum1' */
  rtb_SPDerror_n = rtb_Product5 - rtu_SpeedInms;

  /* Outputs for Atomic SubSystem: '<S2>/Switching Control' */
  /* RelationalOperator: '<S7>/Relational Operator1' incorporates:
   *  Constant: '<S7>/Constant'
   */
  rtb_RelationalOperator1 = (rtb_SPDerror_n > 0.5F);

  /* Switch: '<S7>/Switch' incorporates:
   *  Constant: '<S2>/Switching rpm'
   *  Constant: '<S7>/Constant1'
   *  Constant: '<S7>/Trq min'
   *  Logic: '<S7>/AND'
   *  Logic: '<S7>/AND1'
   *  RelationalOperator: '<S7>/Relational Operator2'
   *  RelationalOperator: '<S7>/Relational Operator3'
   *  Switch: '<S7>/Switch1'
   */
  if (rtb_RelationalOperator1 && (rtu_RPM < 221.936295F)) {
    rtb_Switch = -40.31F;
  } else if (rtb_RelationalOperator1 && (rtu_RPM > 221.936295F)) {
    /* Switch: '<S7>/Switch1' incorporates:
     *  Constant: '<S7>/Trq min limited'
     */
    rtb_Switch = -13.39F;
  } else {
    rtb_Switch = 0.0F;
  }

  /* End of Switch: '<S7>/Switch' */
  /* End of Outputs for SubSystem: '<S2>/Switching Control' */

  /* RelationalOperator: '<S6>/FixPt Relational Operator' incorporates:
   *  UnitDelay: '<S6>/Delay Input1'
   *
   * Block description for '<S6>/Delay Input1':
   *
   *  Store in Global RAM
   */
  rtb_RelationalOperator1 = (rtu_Totaldiff != localDW->DelayInput1_DSTATE);

  /* DiscreteIntegrator: '<S2>/Discrete-Time Integrator' */
  if (rtb_RelationalOperator1 && (localDW->DiscreteTimeIntegrator_PrevRese <= 0))
  {
    localDW->DiscreteTimeIntegrator_DSTATE = 0.0F;
  }

  /* Sum: '<S2>/Sum' incorporates:
   *  Constant: '<S2>/Wind Disturbance Estimation'
   *  DiscreteIntegrator: '<S2>/Discrete-Time Integrator'
   *  Lookup_n-D: '<S2>/n-D Lookup Table3'
   *  Lookup_n-D: '<S2>/n-D Lookup Table4'
   *  Lookup_n-D: '<S2>/n-D Lookup Table5'
   *  Product: '<S2>/Product'
   *  Product: '<S2>/Product1'
   *  Product: '<S2>/Product2'
   *  Sum: '<S2>/Sum3'
   */
  rtb_uDLookupTable = ((localDW->DiscreteTimeIntegrator_DSTATE - rtu_DistanceInm)
                       * look1_iflf_linlcpw(rtu_Laptime,
    code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled3, 1070U) +
                       look1_iflf_linlcpw(rtu_Laptime,
    code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled4, 1070U) *
                       rtb_SPDerror_n) + look1_iflf_linlcpw(rtu_Laptime,
    code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled1, 1070U) * 0.0F;

  /* Saturate: '<S2>/Saturation1' */
  if (rtb_uDLookupTable <= 0.0F) {
    rtb_SPDerror_n = 0.0F;
  } else {
    rtb_SPDerror_n = rtb_uDLookupTable;
  }

  /* End of Saturate: '<S2>/Saturation1' */

  /* Switch: '<S2>/Switch' incorporates:
   *  Constant: '<S2>/Constant1'
   *  Constant: '<S2>/Max RPM'
   *  RelationalOperator: '<S2>/LessThanOrEqual'
   */
  if (rtu_RPM <= 379.489746F) {
    /* Lookup_n-D: '<S2>/n-D Lookup Table8' */
    *rty_TorqueRef = look1_iflf_linlcpw(rtu_DistanceInm,
      code_gen_main_ConstP.pooled6, code_gen_main_ConstP.pooled5, 1070U);

    /* Switch: '<S2>/Switch2' incorporates:
     *  Constant: '<S2>/Constant'
     *  RelationalOperator: '<S2>/Relational Operator'
     */
    if (*rty_TorqueRef > 0.0F) {
      rtb_Sum2 = rtb_SPDerror_n;
    } else {
      rtb_Sum2 = 0.0F;
    }

    /* Sum: '<S2>/Sum2' incorporates:
     *  Switch: '<S2>/Switch2'
     */
    rtb_Sum2 = (*rty_TorqueRef - rtb_Switch) - rtb_Sum2;

    /* Lookup_n-D: '<S2>/1-D Lookup Table' */
    rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM, code_gen_main_ConstP.pooled8,
      code_gen_main_ConstP.pooled7, 49U);

    /* Switch: '<S2>/Switch1' incorporates:
     *  RelationalOperator: '<S2>/LessThanOrEqual1'
     */
    if (rtb_Sum2 <= rtb_uDLookupTable) {
      rtb_uDLookupTable = rtb_Sum2;
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
  rtb_Sum2 = (rtu_RPM * 7.2E-5F + rtb_uDLookupTable * 0.02384F) + 0.007204F;

  /* Saturate: '<S2>/Saturation' */
  if (rtb_Sum2 > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (rtb_Sum2 < 0.0F) {
    *rty_ThrottleOut = 0.0F;
  } else {
    *rty_ThrottleOut = rtb_Sum2;
  }

  /* End of Saturate: '<S2>/Saturation' */

  /* Sum: '<S2>/Sum5' */
  *rty_TorqueGain = rtb_Switch + rtb_SPDerror_n;

  /* Update for UnitDelay: '<S6>/Delay Input1'
   *
   * Block description for '<S6>/Delay Input1':
   *
   *  Store in Global RAM
   */
  localDW->DelayInput1_DSTATE = rtu_Totaldiff;

  /* Update for DiscreteIntegrator: '<S2>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_DSTATE += 0.05F * rtb_Product5;
  localDW->DiscreteTimeIntegrator_PrevRese = (int8_T)rtb_RelationalOperator1;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
