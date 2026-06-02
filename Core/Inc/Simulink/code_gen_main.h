/*
 * Trial License - for use to evaluate programs for possible purchase as
 * an end-user only.
 *
 * File: code_gen_main.h
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

#ifndef code_gen_main_h_
#define code_gen_main_h_
#ifndef code_gen_main_COMMON_INCLUDES_
#define code_gen_main_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* code_gen_main_COMMON_INCLUDES_ */

#include "code_gen_main_types.h"
#include "speed_hold.h"
#include "automatic_strategy.h"

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
  DW_automatic_strategy_T AutomaticStrategy;/* '<Root>/Automatic Strategy' */
} DW_code_gen_main_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: nDLookupTable3_tableData
   * Referenced by: '<S1>/n-D Lookup Table3'
   */
  real32_T nDLookupTable3_tableData[1071];

  /* Pooled Parameter (Mixed Expressions)
   * Referenced by:
   *   '<S1>/n-D Lookup Table3'
   *   '<S1>/n-D Lookup Table4'
   *   '<S1>/n-D Lookup Table5'
   *   '<S1>/n-D Lookup Table6'
   *   '<S3>/n-D Lookup Table6'
   */
  real32_T pooled1[1071];

  /* Computed Parameter: nDLookupTable5_tableData
   * Referenced by: '<S1>/n-D Lookup Table5'
   */
  real32_T nDLookupTable5_tableData[1071];

  /* Computed Parameter: nDLookupTable4_tableData
   * Referenced by: '<S1>/n-D Lookup Table4'
   */
  real32_T nDLookupTable4_tableData[1071];

  /* Pooled Parameter (Expression: AUMOVIO_outline_trq_ref_dat)
   * Referenced by:
   *   '<S1>/n-D Lookup Table8'
   *   '<S3>/n-D Lookup Table8'
   */
  real32_T pooled2[1071];

  /* Pooled Parameter (Expression: AUMOVIO_outline_trq_ref_dist)
   * Referenced by:
   *   '<S1>/n-D Lookup Table8'
   *   '<S3>/n-D Lookup Table8'
   */
  real32_T pooled3[1071];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_trq)
   * Referenced by:
   *   '<S1>/1-D Lookup Table'
   *   '<S3>/1-D Lookup Table'
   */
  real32_T pooled4[50];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_rpm)
   * Referenced by:
   *   '<S1>/1-D Lookup Table'
   *   '<S3>/1-D Lookup Table'
   */
  real32_T pooled5[50];

  /* Pooled Parameter (Expression: AUMOVIO_outline_Spd_ts_Data)
   * Referenced by:
   *   '<S1>/n-D Lookup Table6'
   *   '<S3>/n-D Lookup Table6'
   */
  real32_T pooled11[1071];
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
 * '<S1>'   : 'code_gen_main/Automatic Strategy'
 * '<S2>'   : 'code_gen_main/Speed Hold'
 * '<S3>'   : 'code_gen_main/Switching Automatic Strategy'
 * '<S4>'   : 'code_gen_main/Automatic Strategy/Detect Change'
 * '<S5>'   : 'code_gen_main/Speed Hold/Discrete PID'
 * '<S6>'   : 'code_gen_main/Speed Hold/Discrete PID/Anti-windup'
 * '<S7>'   : 'code_gen_main/Speed Hold/Discrete PID/D Gain'
 * '<S8>'   : 'code_gen_main/Speed Hold/Discrete PID/External Derivative'
 * '<S9>'   : 'code_gen_main/Speed Hold/Discrete PID/Filter'
 * '<S10>'  : 'code_gen_main/Speed Hold/Discrete PID/Filter ICs'
 * '<S11>'  : 'code_gen_main/Speed Hold/Discrete PID/I Gain'
 * '<S12>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain'
 * '<S13>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain Fdbk'
 * '<S14>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator'
 * '<S15>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator ICs'
 * '<S16>'  : 'code_gen_main/Speed Hold/Discrete PID/N Copy'
 * '<S17>'  : 'code_gen_main/Speed Hold/Discrete PID/N Gain'
 * '<S18>'  : 'code_gen_main/Speed Hold/Discrete PID/P Copy'
 * '<S19>'  : 'code_gen_main/Speed Hold/Discrete PID/Parallel P Gain'
 * '<S20>'  : 'code_gen_main/Speed Hold/Discrete PID/Reset Signal'
 * '<S21>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation'
 * '<S22>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation Fdbk'
 * '<S23>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum'
 * '<S24>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum Fdbk'
 * '<S25>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode'
 * '<S26>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode Sum'
 * '<S27>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Integral'
 * '<S28>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Ngain'
 * '<S29>'  : 'code_gen_main/Speed Hold/Discrete PID/postSat Signal'
 * '<S30>'  : 'code_gen_main/Speed Hold/Discrete PID/preInt Signal'
 * '<S31>'  : 'code_gen_main/Speed Hold/Discrete PID/preSat Signal'
 * '<S32>'  : 'code_gen_main/Speed Hold/Discrete PID/Anti-windup/Passthrough'
 * '<S33>'  : 'code_gen_main/Speed Hold/Discrete PID/D Gain/Internal Parameters'
 * '<S34>'  : 'code_gen_main/Speed Hold/Discrete PID/External Derivative/Error'
 * '<S35>'  : 'code_gen_main/Speed Hold/Discrete PID/Filter/Disc. Forward Euler Filter'
 * '<S36>'  : 'code_gen_main/Speed Hold/Discrete PID/Filter ICs/Internal IC - Filter'
 * '<S37>'  : 'code_gen_main/Speed Hold/Discrete PID/I Gain/Internal Parameters'
 * '<S38>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain/Passthrough'
 * '<S39>'  : 'code_gen_main/Speed Hold/Discrete PID/Ideal P Gain Fdbk/Disabled'
 * '<S40>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator/Discrete'
 * '<S41>'  : 'code_gen_main/Speed Hold/Discrete PID/Integrator ICs/Internal IC'
 * '<S42>'  : 'code_gen_main/Speed Hold/Discrete PID/N Copy/Disabled'
 * '<S43>'  : 'code_gen_main/Speed Hold/Discrete PID/N Gain/Internal Parameters'
 * '<S44>'  : 'code_gen_main/Speed Hold/Discrete PID/P Copy/Disabled'
 * '<S45>'  : 'code_gen_main/Speed Hold/Discrete PID/Parallel P Gain/Internal Parameters'
 * '<S46>'  : 'code_gen_main/Speed Hold/Discrete PID/Reset Signal/Disabled'
 * '<S47>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation/Enabled'
 * '<S48>'  : 'code_gen_main/Speed Hold/Discrete PID/Saturation Fdbk/Disabled'
 * '<S49>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum/Sum_PID'
 * '<S50>'  : 'code_gen_main/Speed Hold/Discrete PID/Sum Fdbk/Disabled'
 * '<S51>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode/Disabled'
 * '<S52>'  : 'code_gen_main/Speed Hold/Discrete PID/Tracking Mode Sum/Passthrough'
 * '<S53>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Integral/TsSignalSpecification'
 * '<S54>'  : 'code_gen_main/Speed Hold/Discrete PID/Tsamp - Ngain/Passthrough'
 * '<S55>'  : 'code_gen_main/Speed Hold/Discrete PID/postSat Signal/Forward_Path'
 * '<S56>'  : 'code_gen_main/Speed Hold/Discrete PID/preInt Signal/Internal PreInt'
 * '<S57>'  : 'code_gen_main/Speed Hold/Discrete PID/preSat Signal/Forward_Path'
 * '<S58>'  : 'code_gen_main/Switching Automatic Strategy/Switching Control'
 */
#endif                                 /* code_gen_main_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
