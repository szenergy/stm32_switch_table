/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: auto_strat_C_generate.h
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

#ifndef auto_strat_C_generate_h_
#define auto_strat_C_generate_h_
#ifndef auto_strat_C_generate_COMMON_INCLUDES_
#define auto_strat_C_generate_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                              /* auto_strat_C_generate_COMMON_INCLUDES_ */

#include "auto_strat_C_generate_types.h"
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
  DW_automatic_strategy_T AutomaticStrategy;/* '<Root>/Automatic Strategy' */
} DW_auto_strat_C_generate_T;

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
   *   '<S2>/n-D Lookup Table6'
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

  /* Pooled Parameter (Expression: trq_ref_dat)
   * Referenced by:
   *   '<S1>/n-D Lookup Table8'
   *   '<S2>/n-D Lookup Table8'
   */
  real32_T pooled2[1071];

  /* Pooled Parameter (Expression: trq_ref_dist)
   * Referenced by:
   *   '<S1>/n-D Lookup Table8'
   *   '<S2>/n-D Lookup Table8'
   */
  real32_T pooled3[1071];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_trq)
   * Referenced by:
   *   '<S1>/1-D Lookup Table'
   *   '<S2>/1-D Lookup Table'
   */
  real32_T pooled4[50];

  /* Pooled Parameter (Expression: SZEVOL_2_1_PESC1_MTPA_FW69_22_lim_rpm)
   * Referenced by:
   *   '<S1>/1-D Lookup Table'
   *   '<S2>/1-D Lookup Table'
   */
  real32_T pooled5[50];

  /* Pooled Parameter (Expression: AUMOVIO_outline_Spd_ts_Data)
   * Referenced by:
   *   '<S1>/n-D Lookup Table6'
   *   '<S2>/n-D Lookup Table6'
   */
  real32_T pooled11[1071];
} ConstP_auto_strat_C_generate_T;

/* Real-time Model Data Structure */
struct tag_RTM_auto_strat_C_generate_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_auto_strat_C_generate_T auto_strat_C_generate_DW;

/* Constant parameters (default storage) */
extern const ConstP_auto_strat_C_generate_T auto_strat_C_generate_ConstP;

/* Model entry point functions */
extern void auto_strat_C_generate_initialize(void);
extern void auto_strat_C_generate_step(void);
extern void auto_strat_C_generate_terminate(void);

/* Real-time Model object */
extern RT_MODEL_auto_strat_C_generat_T *const auto_strat_C_generate_M;

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
 * '<Root>' : 'auto_strat_C_generate'
 * '<S1>'   : 'auto_strat_C_generate/Automatic Strategy'
 * '<S2>'   : 'auto_strat_C_generate/Switching Automatic Strategy'
 * '<S3>'   : 'auto_strat_C_generate/Automatic Strategy/Detect Change'
 * '<S4>'   : 'auto_strat_C_generate/Switching Automatic Strategy/Switching Control'
 */
#endif                                 /* auto_strat_C_generate_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
