/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: automatic_strategy.c
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

#include "automatic_strategy.h"
#include "rtwtypes.h"
#include "code_gen_main.h"
#include "code_gen_main_private.h"

/* System initialize for atomic system: '<Root>/Automatic Strategy' */
void automatic_strategy_Init(DW_automatic_strategy_T *localDW)
{
  /* InitializeConditions for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_PrevRese = 2;
}

/* Output and update for atomic system: '<Root>/Automatic Strategy' */
void automatic_strategy(real32_T rtu_SpeedInms, real32_T rtu_DistanceInm,
  real32_T rtu_Laptime, real32_T rtu_RPM, real32_T rtu_Totaldiff, real32_T
  *rty_ThrottleOut, real32_T *rty_TorqueGain, real32_T *rty_TorqueRef,
  DW_automatic_strategy_T *localDW)
{
  real32_T rtb_Product5;
  real32_T rtb_Sum2;
  real32_T rtb_uDLookupTable;
  boolean_T rtb_FixPtRelationalOperator;

  /* Product: '<S1>/Product5' incorporates:
   *  Constant: '<S1>/Constant2'
   *  Lookup_n-D: '<S1>/n-D Lookup Table6'
   *  Product: '<S1>/Divide'
   *  Sum: '<S1>/Sum6'
   */
  rtb_Product5 = (rtu_Totaldiff + 107.0F) / 107.0F * look1_iflf_linlcpw
    (rtu_Laptime, code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled14,
     1070U);

  /* RelationalOperator: '<S5>/FixPt Relational Operator' incorporates:
   *  UnitDelay: '<S5>/Delay Input1'
   *
   * Block description for '<S5>/Delay Input1':
   *
   *  Store in Global RAM
   */
  rtb_FixPtRelationalOperator = (rtu_Totaldiff != localDW->DelayInput1_DSTATE);

  /* DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  if (rtb_FixPtRelationalOperator && (localDW->DiscreteTimeIntegrator_PrevRese <=
       0)) {
    localDW->DiscreteTimeIntegrator_DSTATE = 0.0F;
  }

  /* Switch: '<S1>/Switch' incorporates:
   *  Constant: '<S1>/Constant1'
   *  Constant: '<S1>/Max RPM'
   *  RelationalOperator: '<S1>/LessThanOrEqual'
   */
  if (rtu_RPM <= 379.489746F) {
    /* Lookup_n-D: '<S1>/n-D Lookup Table8' */
    *rty_TorqueRef = look1_iflf_linlcpw(rtu_DistanceInm,
      code_gen_main_ConstP.pooled6, code_gen_main_ConstP.pooled5, 1070U);

    /* Switch: '<S1>/Switch2' incorporates:
     *  Constant: '<S1>/Constant'
     *  RelationalOperator: '<S1>/Relational Operator'
     */
    if (*rty_TorqueRef > 0.0F) {
      /* Sum: '<S1>/Sum' incorporates:
       *  Constant: '<S1>/Wind Disturbance Estimation'
       *  DiscreteIntegrator: '<S1>/Discrete-Time Integrator'
       *  Lookup_n-D: '<S1>/n-D Lookup Table3'
       *  Lookup_n-D: '<S1>/n-D Lookup Table4'
       *  Lookup_n-D: '<S1>/n-D Lookup Table5'
       *  Product: '<S1>/Product'
       *  Product: '<S1>/Product1'
       *  Product: '<S1>/Product2'
       *  Sum: '<S1>/Sum1'
       *  Sum: '<S1>/Sum3'
       */
      *rty_TorqueGain = ((rtb_Product5 - rtu_SpeedInms) * look1_iflf_linlcpw
                         (rtu_Laptime, code_gen_main_ConstP.pooled2,
                          code_gen_main_ConstP.pooled4, 1070U) +
                         (localDW->DiscreteTimeIntegrator_DSTATE -
                          rtu_DistanceInm) * look1_iflf_linlcpw(rtu_Laptime,
        code_gen_main_ConstP.pooled2, code_gen_main_ConstP.pooled3, 1070U)) +
        look1_iflf_linlcpw(rtu_Laptime, code_gen_main_ConstP.pooled2,
                           code_gen_main_ConstP.pooled1, 1070U) * 0.0F;
      rtb_Sum2 = *rty_TorqueGain;
    } else {
      rtb_Sum2 = 0.0F;
    }

    /* End of Switch: '<S1>/Switch2' */

    /* Sum: '<S1>/Sum2' */
    rtb_Sum2 = *rty_TorqueRef - rtb_Sum2;

    /* Lookup_n-D: '<S1>/1-D Lookup Table' */
    rtb_uDLookupTable = look1_iflf_binlxpw(rtu_RPM, code_gen_main_ConstP.pooled8,
      code_gen_main_ConstP.pooled7, 49U);

    /* Switch: '<S1>/Switch1' incorporates:
     *  RelationalOperator: '<S1>/LessThanOrEqual1'
     */
    if (rtb_Sum2 <= rtb_uDLookupTable) {
      rtb_uDLookupTable = rtb_Sum2;
    }

    /* End of Switch: '<S1>/Switch1' */
  } else {
    rtb_uDLookupTable = 0.0F;
  }

  /* End of Switch: '<S1>/Switch' */

  /* Sum: '<S1>/Sum4' incorporates:
   *  Constant: '<S1>/Constant3'
   *  Constant: '<S1>/Constant4'
   *  Constant: '<S1>/Constant5'
   *  Product: '<S1>/Product3'
   *  Product: '<S1>/Product4'
   */
  rtb_Sum2 = (rtu_RPM * 7.2E-5F + rtb_uDLookupTable * 0.02384F) + 0.007204F;

  /* Saturate: '<S1>/Saturation' */
  if (rtb_Sum2 > 1.0F) {
    *rty_ThrottleOut = 1.0F;
  } else if (rtb_Sum2 < 0.0F) {
    *rty_ThrottleOut = 0.0F;
  } else {
    *rty_ThrottleOut = rtb_Sum2;
  }

  /* End of Saturate: '<S1>/Saturation' */

  /* Update for UnitDelay: '<S5>/Delay Input1'
   *
   * Block description for '<S5>/Delay Input1':
   *
   *  Store in Global RAM
   */
  localDW->DelayInput1_DSTATE = rtu_Totaldiff;

  /* Update for DiscreteIntegrator: '<S1>/Discrete-Time Integrator' */
  localDW->DiscreteTimeIntegrator_DSTATE += 0.05F * rtb_Product5;
  localDW->DiscreteTimeIntegrator_PrevRese = (int8_T)rtb_FixPtRelationalOperator;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
