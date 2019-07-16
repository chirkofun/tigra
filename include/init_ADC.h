#ifndef INCLUDE_INIT_ADC_H_
#define INCLUDE_INIT_ADC_H_

#include <common.h>
#include <tf_calculation.h>
#include <motCtrl.h>
#include <ADC.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

static const SerialConfig sdcfg;


int16_t getT(void);
int16_t getK(void);
void changeParamK(int16_t kk);
void changeParamT(int16_t tt);

void transm (void);
void smooth(int16_t *input, int16_t *output, int n, int window);

int getADC(void);
int getSpd(void);
//static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n);
//static void adcerrorcallback(ADCDriver *adcp, adcerror_t err);

void initialization(void);

#endif
