#ifndef INCLUDE_MOT_CTRL_H_
#define INCLUDE_MOT_CTRL_H_


#include <init_ADC.h>
#include <common.h>
#define NfVL      -100
#define NfL       -80
#define NfM       -50
#define NfS       -30
#define NfSS      -15
#define fNO       0
#define PfSS      15
#define PfS       30
#define PfM       50
#define PfL       80
#define PfVL      100

#define fVLN      -125 //Очень большое отрицательное
#define fLN       -90 //Большое отрицательное
#define fMN       -55 //Среднее отрицательное
#define fSN       -20 //Малое отрицательное
#define fNO       0 //Нулевое
#define fSP       20 //Малое положительное
#define fMP       55 //Среднее положительное
#define fLP       90 //Большое положительное
#define fVLP      125 //Очень сильное положительное

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define DC_MIN_PWM 5000
#define DC_MAX_PWM 10000

typedef enum {
    AND,
    OR
} op_type;

void BrakeFunc(void);

int16_t transControlCurrent(void);

int16_t transOut(void);

double mu(int16_t x, int16_t A);

void addrule(int16_t fe, op_type op, int16_t fde, int16_t z);

double getFuzzyConclusion(int16_t e,int16_t de);

void brakeUnitCSSetPower( int16_t pressPower );

void Fuzzy_Init(void);

#endif
