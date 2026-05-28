/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: Automatic.h
 *
 * Code generated for Simulink model 'Automatic'.
 *
 * Model version                  : 1.11
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Thu May 28 13:51:36 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef Automatic_h_
#define Automatic_h_
#ifndef Automatic_COMMON_INCLUDES_
#define Automatic_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* Automatic_COMMON_INCLUDES_ */

#include "Automatic_types.h"
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
} DW_Automatic_T;

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: nDLookupTable8_tableData
   * Referenced by: '<S1>/n-D Lookup Table8'
   */
  real32_T nDLookupTable8_tableData[1071];

  /* Computed Parameter: nDLookupTable8_bp01Data
   * Referenced by: '<S1>/n-D Lookup Table8'
   */
  real32_T nDLookupTable8_bp01Data[1071];

  /* Computed Parameter: uDLookupTable_tableData
   * Referenced by: '<S1>/1-D Lookup Table'
   */
  real32_T uDLookupTable_tableData[50];

  /* Computed Parameter: uDLookupTable_bp01Data
   * Referenced by: '<S1>/1-D Lookup Table'
   */
  real32_T uDLookupTable_bp01Data[50];

  /* Computed Parameter: nDLookupTable4_tableData
   * Referenced by: '<S1>/n-D Lookup Table4'
   */
  real32_T nDLookupTable4_tableData[1071];

  /* Pooled Parameter (Mixed Expressions)
   * Referenced by:
   *   '<S1>/n-D Lookup Table3'
   *   '<S1>/n-D Lookup Table4'
   *   '<S1>/n-D Lookup Table5'
   *   '<S1>/n-D Lookup Table6'
   */
  real32_T pooled1[1071];

  /* Computed Parameter: nDLookupTable6_tableData
   * Referenced by: '<S1>/n-D Lookup Table6'
   */
  real32_T nDLookupTable6_tableData[1071];

  /* Computed Parameter: nDLookupTable5_tableData
   * Referenced by: '<S1>/n-D Lookup Table5'
   */
  real32_T nDLookupTable5_tableData[1071];

  /* Computed Parameter: nDLookupTable3_tableData
   * Referenced by: '<S1>/n-D Lookup Table3'
   */
  real32_T nDLookupTable3_tableData[1071];
} ConstP_Automatic_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T SpeedInms;                  /* '<Root>/Speed In (m//s)' */
  real32_T DistanceInm;                /* '<Root>/Distance In (m)' */
  real32_T Laptime;                    /* '<Root>/Lap time' */
  real32_T RPM;                        /* '<Root>/RPM' */
  real32_T Previoslaptime;             /* '<Root>/Previos lap time' */
} ExtU_Automatic_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real32_T ThrottleOut;                /* '<Root>/Throttle Out' */
  real32_T TorqueGain;                 /* '<Root>/Torque Gain' */
  real32_T TorqueRef;                  /* '<Root>/Torque Ref' */
} ExtY_Automatic_T;

/* Real-time Model Data Structure */
struct tag_RTM_Automatic_T {
  const char_T * volatile errorStatus;
};

/* Block states (default storage) */
extern DW_Automatic_T Automatic_DW;

/* External inputs (root inport signals with default storage) */
extern ExtU_Automatic_T Automatic_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_Automatic_T Automatic_Y;

/* Constant parameters (default storage) */
extern const ConstP_Automatic_T Automatic_ConstP;

/* Model entry point functions */
extern void Automatic_initialize(void);
extern void Automatic_step(void);
extern void Automatic_terminate(void);

/* Real-time Model object */
extern RT_MODEL_Automatic_T *const Automatic_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('test_model/Automatic Strategy')    - opens subsystem test_model/Automatic Strategy
 * hilite_system('test_model/Automatic Strategy/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'test_model'
 * '<S1>'   : 'test_model/Automatic Strategy'
 * '<S2>'   : 'test_model/Automatic Strategy/Detect Change'
 */
#endif                                 /* Automatic_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
