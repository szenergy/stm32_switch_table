/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: code_gen_main.c
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

#include "code_gen_main.h"
#include "automatic_strategy.h"
#include "speed_hold.h"
#include "switching_automatic_strategy.h"
#include "rtwtypes.h"
#include "code_gen_main_private.h"

/* Block states (default storage) */
DW_code_gen_main_T code_gen_main_DW;

/* Real-time model */
static RT_MODEL_code_gen_main_T code_gen_main_M_;
RT_MODEL_code_gen_main_T *const code_gen_main_M = &code_gen_main_M_;
real32_T look1_iflf_linlcpw(real32_T u0, const real32_T bp0[], const real32_T
  table[], uint32_T maxIndex)
{
  real32_T frac;
  real32_T yL_0d0;
  uint32_T bpIdx;

  /* Column-major Lookup 1-D
     Search method: 'linear'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Clip'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'linear'
     Extrapolation method: 'Clip'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    bpIdx = 0U;
    frac = 0.0F;
  } else if (u0 < bp0[maxIndex]) {
    /* Linear Search */
    for (bpIdx = maxIndex >> 1U; u0 < bp0[bpIdx]; bpIdx--) {
    }

    while (u0 >= bp0[bpIdx + 1U]) {
      bpIdx++;
    }

    frac = (u0 - bp0[bpIdx]) / (bp0[bpIdx + 1U] - bp0[bpIdx]);
  } else {
    bpIdx = maxIndex - 1U;
    frac = 1.0F;
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  yL_0d0 = table[bpIdx];
  return (table[bpIdx + 1U] - yL_0d0) * frac + yL_0d0;
}

real32_T look1_iflf_binlxpw(real32_T u0, const real32_T bp0[], const real32_T
  table[], uint32_T maxIndex)
{
  real32_T frac;
  real32_T yL_0d0;
  uint32_T iLeft;

  /* Column-major Lookup 1-D
     Search method: 'binary'
     Use previous index: 'off'
     Interpolation method: 'Linear point-slope'
     Extrapolation method: 'Linear'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  /* Prelookup - Index and Fraction
     Index Search method: 'binary'
     Extrapolation method: 'Linear'
     Use previous index: 'off'
     Use last breakpoint for index at or above upper limit: 'off'
     Remove protection against out-of-range input in generated code: 'off'
   */
  if (u0 <= bp0[0U]) {
    iLeft = 0U;
    frac = (u0 - bp0[0U]) / (bp0[1U] - bp0[0U]);
  } else if (u0 < bp0[maxIndex]) {
    uint32_T bpIdx;
    uint32_T iRght;

    /* Binary Search */
    bpIdx = maxIndex >> 1U;
    iLeft = 0U;
    iRght = maxIndex;
    while (iRght - iLeft > 1U) {
      if (u0 < bp0[bpIdx]) {
        iRght = bpIdx;
      } else {
        iLeft = bpIdx;
      }

      bpIdx = (iRght + iLeft) >> 1U;
    }

    frac = (u0 - bp0[iLeft]) / (bp0[iLeft + 1U] - bp0[iLeft]);
  } else {
    iLeft = maxIndex - 1U;
    frac = (u0 - bp0[maxIndex - 1U]) / (bp0[maxIndex] - bp0[maxIndex - 1U]);
  }

  /* Column-major Interpolation 1-D
     Interpolation method: 'Linear point-slope'
     Use last breakpoint for index at or above upper limit: 'off'
     Overflow mode: 'portable wrapping'
   */
  yL_0d0 = table[iLeft];
  return (table[iLeft + 1U] - yL_0d0) * frac + yL_0d0;
}

/* Model step function */
void code_gen_main_step(void)
{
  real32_T rtb_ThrottleOut_a;
  real32_T rtb_TorqueGain;
  real32_T rtb_TorqueRef_k;

  /* Outputs for Atomic SubSystem: '<Root>/Automatic Strategy' */
  automatic_strategy(0.0F, 0.0F, 0.0F, 0.0F, 0.0F, &rtb_ThrottleOut_a,
                     &rtb_TorqueGain, &rtb_TorqueRef_k,
                     &code_gen_main_DW.AutomaticStrategy);

  /* End of Outputs for SubSystem: '<Root>/Automatic Strategy' */

  /* Outputs for Atomic SubSystem: '<Root>/Speed Hold' */
  speed_hold(0.0F, 0.0F, &rtb_ThrottleOut_a, &rtb_TorqueGain,
             &code_gen_main_DW.SpeedHold);

  /* End of Outputs for SubSystem: '<Root>/Speed Hold' */

  /* Outputs for Atomic SubSystem: '<Root>/Switching Automatic Strategy' */
  switching_automatic_strategy(0.0F, 0.0F, 0.0F, 0.0F, 0.0F, &rtb_ThrottleOut_a,
    &rtb_TorqueGain, &rtb_TorqueRef_k);

  /* End of Outputs for SubSystem: '<Root>/Switching Automatic Strategy' */
}

/* Model initialize function */
void code_gen_main_initialize(void)
{
  /* SystemInitialize for Atomic SubSystem: '<Root>/Automatic Strategy' */
  automatic_strategy_Init(&code_gen_main_DW.AutomaticStrategy);

  /* End of SystemInitialize for SubSystem: '<Root>/Automatic Strategy' */
}

/* Model terminate function */
void code_gen_main_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
