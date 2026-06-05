/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: code_gen_main.h
 *
 * Code generated for Simulink model 'code_gen_main'.
 *
 * Model version                  : 1.25
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Fri Jun  5 09:51:16 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef code_gen_main_h_
#define code_gen_main_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

#include "code_gen_main_types.h"
#include "speed_hold.h"
#include "ltv_lqr_strategy.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block states (default storage) for system '<Root>' */
typedef struct {
  DW_speed_hold_T SpeedHold;           /* '<Root>/Speed Hold' */
  DW_ltv_lqr_strategy_T LTVLQRStrategy;/* '<Root>/LTV - LQR Strategy' */
} DW_code_gen_main_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: uDK1x2lookup_tableData
   * Referenced by: '<S1>/1D K1 x2 lookup'
   */
  real32_T uDK1x2lookup_tableData[1071];

  /* Pooled Parameter (Mixed Expressions)
   * Referenced by:
   *   '<S1>/1D K1 wff lookup'
   *   '<S1>/1D K1 x1 lookup'
   *   '<S1>/1D K1 x2 lookup'
   *   '<S1>/1D Speed from laptime'
   */
  real32_T pooled1[1071];

  /* Computed Parameter: uDK1x1lookup_tableData
   * Referenced by: '<S1>/1D K1 x1 lookup'
   */
  real32_T uDK1x1lookup_tableData[1071];

  /* Computed Parameter: uDK1wfflookup_tableData
   * Referenced by: '<S1>/1D K1 wff lookup'
   */
  real32_T uDK1wfflookup_tableData[1071];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_trq)
   * Referenced by:
   *   '<S1>/1D PESC Lim Torque'
   *   '<S3>/1-D Lookup Table'
   */
  real32_T pooled2[50];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_rpm)
   * Referenced by:
   *   '<S1>/1D PESC Lim Torque'
   *   '<S3>/1-D Lookup Table'
   */
  real32_T pooled3[50];

  /* Computed Parameter: uDSpeedfromlaptime_tableData
   * Referenced by: '<S1>/1D Speed from laptime'
   */
  real32_T uDSpeedfromlaptime_tableData[1071];

  /* Pooled Parameter (Expression: AUMOVIO_outline_trq_ref_dat)
   * Referenced by:
   *   '<S1>/1D Torque from Distance'
   *   '<S3>/norm'
   */
  real32_T pooled13[1071];

  /* Pooled Parameter (Expression: AUMOVIO_outline_trq_ref_dist)
   * Referenced by:
   *   '<S1>/1D Torque from Distance'
   *   '<S3>/fast'
   *   '<S3>/norm'
   *   '<S3>/slow'
   */
  real32_T pooled14[1071];

  /* Computed Parameter: slow_tableData
   * Referenced by: '<S3>/slow'
   */
  real32_T slow_tableData[1071];

  /* Computed Parameter: fast_tableData
   * Referenced by: '<S3>/fast'
   */
  real32_T fast_tableData[1071];
} ConstP_code_gen_main_T;

/* Real-time Model Data Structure */
struct tag_RTM_code_gen_main_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_code_gen_main_T code_gen_main_DW;

/* Constant parameters (default storage) */
extern const ConstP_code_gen_main_T code_gen_main_ConstP;

/* Model entry point functions */
extern void code_gen_main_initialize(void);
extern void code_gen_main_step(void);
extern void code_gen_main_terminate(void);

/* Real-time Model object */
extern RT_MODEL_code_gen_main_T *const code_gen_main_M;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Switching rpm' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'code_gen_main'
 * '<S1>'   : 'code_gen_main/LTV - LQR Strategy'
 * '<S2>'   : 'code_gen_main/Speed Hold'
 * '<S3>'   : 'code_gen_main/Switching LQR Strategy'
 * '<S4>'   : 'code_gen_main/Speed Hold/Discrete PID'
 * '<S5>'   : 'code_gen_main/Speed Hold/Discrete PID/Anti-windup'
 * '<S6>'   : 'code_gen_main/Speed Hold/Discrete PID/D Gain'
 * '<S7>'   : 'code_gen_main/Speed Hold/Discrete PID/External Derivative'
 * '<S8>'   : 'code_gen_main/Speed Hold/Discrete PID/Filter'
 * '<S9>'   : 'code_gen_main/Speed Hold/Discrete PID/Filter ICs'
 * '<S10>'  : 'code_gen_main/Speed Hold/Discrete PID/I Gain'
 * '<S11>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain'
 * '<S12>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain Fdbk'
 * '<S13>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator'
 * '<S14>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator ICs'
 * '<S15>'  : 'code_gen_main/Speed Hold/Discrete PID/N Copy'
 * '<S16>'  : 'code_gen_main/Speed Hold/Discrete PID/N Gain'
 * '<S17>'  : 'code_gen_main/Speed Hold/Discrete PID/P Copy'
 * '<S18>'  : 'code_gen_main/Speed Hold/Discrete PID/Parallel P Gain'
 * '<S19>'  : 'code_gen_main/Speed Hold/Discrete PID/Reset Signal'
 * '<S20>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation'
 * '<S21>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation Fdbk'
 * '<S22>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum'
 * '<S23>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum Fdbk'
 * '<S24>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode'
 * '<S25>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode Sum'
 * '<S26>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Integral'
 * '<S27>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Ngain'
 * '<S28>'  : 'code_gen_main/Speed Hold/Discrete PID/postSat Signal'
 * '<S29>'  : 'code_gen_main/Speed Hold/Discrete PID/preInt Signal'
 * '<S30>'  : 'code_gen_main/Speed Hold/Discrete PID/preSat Signal'
 * '<S31>'  : 'code_gen_main/Speed Hold/Discrete PID/Anti-windup/Passthrough'
 * '<S32>'  : 'code_gen_main/Speed Hold/Discrete PID/D Gain/Internal Parameters'
 * '<S33>'  : 'code_gen_main/Speed Hold/Discrete PID/External Derivative/Error'
 * '<S34>'  : 'code_gen_main/Speed Hold/Discrete PID/Filter/Disc. Forward Euler Filter'
 * '<S35>'  : 'code_gen_main/Speed Hold/Discrete PID/Filter ICs/Internal IC - Filter'
 * '<S36>'  : 'code_gen_main/Speed Hold/Discrete PID/I Gain/Internal Parameters'
 * '<S37>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain/Passthrough'
 * '<S38>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain Fdbk/Disabled'
 * '<S39>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator/Discrete'
 * '<S40>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator ICs/Internal IC'
 * '<S41>'  : 'code_gen_main/Speed Hold/Discrete PID/N Copy/Disabled'
 * '<S42>'  : 'code_gen_main/Speed Hold/Discrete PID/N Gain/Internal Parameters'
 * '<S43>'  : 'code_gen_main/Speed Hold/Discrete PID/P Copy/Disabled'
 * '<S44>'  : 'code_gen_main/Speed Hold/Discrete PID/Parallel P Gain/Internal Parameters'
 * '<S45>'  : 'code_gen_main/Speed Hold/Discrete PID/Reset Signal/Disabled'
 * '<S46>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation/Enabled'
 * '<S47>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation Fdbk/Disabled'
 * '<S48>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum/Sum_PID'
 * '<S49>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum Fdbk/Disabled'
 * '<S50>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode/Disabled'
 * '<S51>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode Sum/Passthrough'
 * '<S52>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Integral/TsSignalSpecification'
 * '<S53>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Ngain/Passthrough'
 * '<S54>'  : 'code_gen_main/Speed Hold/Discrete PID/postSat Signal/Forward_Path'
 * '<S55>'  : 'code_gen_main/Speed Hold/Discrete PID/preInt Signal/Internal PreInt'
 * '<S56>'  : 'code_gen_main/Speed Hold/Discrete PID/preSat Signal/Forward_Path'
 */
#endif                                 /* code_gen_main_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
