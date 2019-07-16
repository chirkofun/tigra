#ifndef INCLUDE_TF_CALCULATION_H_
#define INCLUDE_TF_CALCULATION_H_

/*
 * @brief      Transfer function structure.
 *             Parameters and state of difference equation.
 *             W(s) = k / ( T*s + 1 )
 */
struct TFConf {
  /* gain value (parameter)      */
  float   k;

  /* transfer function input    */
  float input;

};

typedef struct TFConf TFConf_t;


void tfOutCalculation ( TFConf_t * );


#endif
