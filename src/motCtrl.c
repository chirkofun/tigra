#include <motCtrl.h>

typedef float       controllerRate_t;
typedef float       controllerError_t;
typedef float       controllerRensponse_t;

/* Configuration - PI controller parameters*/
typedef struct
{
    /* User defined parameters */
    controllerRate_t    kp;
    controllerRate_t    ki;
    controllerRate_t    kd;
    float               integrLimit;
    controllerError_t   err;

    /* Service parameters */
    float               integrSum;
    controllerError_t   prevErr;

    float               integZone; // in %/100
    float               integZone_abs; // reference * %

} PIControllerContext_t;


/*** Prototypes ***/

/**
 * @brief   PID controller context initialization
 * @params  ctx      PID controller context
 * @note    Check user defined parameters
 */
void PIControlInit ( PIControllerContext_t *ctx );

/**
 * @brief       Control system law realization. PID controller *
 * @params[in]  ctx     PID controller context
 * @params[out] Controller output <controllerRensponse_t>
 */
controllerRensponse_t PIControlResponse ( PIControllerContext_t *ctx );


uint8_t numofrules;

struct rule {
    int16_t fe;
    op_type op;
    int16_t fde;
    int16_t z;
};
struct rule rules[20];

static double summ_alpha_c = 0;
static double summ_alpha = 0;
static double alpha = 0, mue = 0, mude = 0;
static double result = 0;
int16_t controlCurrent=0;
double koeff = 1;
int16_t Speed = DC_MIN_PWM;
float outp = 0;
double dMTh = 0;
int16_t currentNow=0;
int16_t zeroPoint = 2048;
int16_t error = 0;
int16_t iPart = 0;
int16_t countt =0;
int16_t iprevRollError = 0;
int16_t iDeltaError = 0;
int16_t SpeedPWM = 0;
int Power = 0;


void setZeroPoint(int16_t inputParam)
{
    zeroPoint = inputParam;
}

static PIControllerContext_t  pidCtx = {
    .kp   = 0.15,
    .ki   = 0.08,
    .kd   = 0,
    .integrLimit  = 1000,
    .integZone = 1.0
};

int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static TFConf_t tfconf = {
                   .k             = 1,
                   .input         = 0
};

static TFConf_t *p_tfconf = &tfconf;

void BrakeFunc(void)
{
    //Power = tfconf.k > 0 ? tfconf.k

    if ( tfconf.k > 0 )
    {
         tfconf.input = 1;
         currentNow = getADC();
         outp = tfconf.k;

         currentNow = map((currentNow-zeroPoint), -2048, 2048, -100, 100); // 100 - 4.8 A
         error = (outp - currentNow);
         controlCurrent = constrain(error, -100, 100);
         if(outp != 0)
         {
                 pidCtx.err = controlCurrent;

                 Speed = PIControlResponse( &pidCtx );
                 Speed = constrain(Speed,0,1000);

                 SpeedPWM = map(Speed,0,1000,5500,10000);
         }
         pwmEnableChannel( &PWMD2, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, SpeedPWM) );
    }
    else if( tfconf.k < 0 )
    {
        /* Set return power */
        if ( brakeSensorIsPressed() )
        {
            tfconf.input = 0;
            pwmEnableChannel( &PWMD2, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, 6000) );
        }
        else
        {
          tfconf.input = 0;
          pidCtx.err =0;
          pidCtx.prevErr =0;
          pidCtx.integrSum=0;
          tfconf.k = 0;
          pwmEnableChannel( &PWMD2, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD2, 0) );
        }
     }
     else
     {
         tfconf.input = 0;
         pidCtx.err =0;
         pidCtx.prevErr =0;
         pidCtx.integrSum=0;
         tfconf.k = 0;
     }

    /*if (outp == 0)
    {
        zeroPoint += getADC();
        zeroPoint /= 2;
    }*/
}


double mu(int16_t x, int16_t A) {
  if(A == fNO)
  {
    return exp((-(x - A)*(x - A)) / 200);
  }
  if(A == NfS || A == PfS)
  {
    return exp((-(x - A)*(x - A)) / 8000);
  }
  if(A == NfSS || A == PfSS)
  {
    return exp((-(x - A)*(x - A)) / 10000);
  }
  if(A == NfM || A == PfM)
  {
    return exp((-(x - A)*(x - A)) / 1500);
  }
  if(A == NfL || A == PfL)
  {
    return exp((-(x - A)*(x - A)) / 1500);
  }
  if(A == NfVL || A == PfVL)
  {
    return exp((-(x - A)*(x - A)) / 2000);
  }
  else
  {
    return 0;
  }
}


void addrule(int16_t fe, op_type op, int16_t fde, int16_t z) {
    rules[numofrules].fe = fe;
    rules[numofrules].op = op;
    rules[numofrules].fde = fde;
    rules[numofrules].z = z;
    numofrules++;
}



double getFuzzyConclusion(int16_t e, int16_t de) {
  int8_t i = 0;
  summ_alpha_c = 0;
  summ_alpha = 0;
    for (i = 0; i < numofrules; i++)
    {
      mue = mu(e, rules[i].fe);
      mude = mu(de, rules[i].fde);
      //alpha = rules[i].op == 0 ? MIN(mue, mude) : MAX(mue, mude);
      alpha = mue;
      summ_alpha_c += (alpha * rules[i].z);
      summ_alpha += alpha;
    }
    result = -summ_alpha_c/summ_alpha;
    return result;
}

void Fuzzy_Init()
{
    numofrules = 0;

    addrule(fNO, AND, fNO, fNO);

        addrule(PfSS,AND, fNO, NfSS);
        addrule(PfS,AND, fNO, NfS);
        addrule(PfM,AND, fNO, NfM);
        addrule(PfL,AND, fNO, NfL);
        addrule(PfVL,AND, fNO, NfVL);

        addrule(NfSS,AND, fNO, PfSS);
        addrule(NfS,AND, fNO, PfS);
        addrule(NfM,AND, fNO, PfM);
        addrule(NfL,AND, fNO, PfL);
        addrule(NfVL,AND, fNO, PfVL);

    //addrule(NfSS, AND, NfSS, NfSS);
    //addrule(PfSS, AND, PfSS, PfSS);
    //addrule(PfSS, AND, PfS, NfSS);
    //addrule(NfSS, AND, fNO, NfS);
    //addrule(PfSS, AND, fNO, PfS);

    /*
     * Last PD
     * addrule(NfS, AND, NfS, NfSS);
    addrule(PfS, AND, PfS, PfSS);

    addrule(NfS, AND, fNO, NfSS);
    addrule(PfS, AND, NfSS, PfSS);

    addrule(NfS, AND, fNO, NfSS);
    addrule(PfS, AND, fNO, PfSS);

    addrule(NfM, AND, NfS, NfS);
    addrule(PfM, AND, PfS, PfS);*/

    /*addrule(PfS, AND, fNO, NfSS);
    addrule(PfS, AND, PfSS, NfSS);
    addrule(NfSS, AND, fNO, PfSS);
    addrule(NfSS, AND, NfSS, PfSS);
    addrule(PfS, AND, NfS, NfSS);
    addrule(PfM, AND, fNO, NfM);*/

    /*addrule(fVLN, OR, fVLN, fVLP);
    addrule(fVLP, OR, fVLP, fVLN);
    addrule(fLN, AND, fSN, fVLP);
    addrule(fLP, AND, fSP, fVLN);
    addrule(fMN, AND, fMP, fSP);
    addrule(fMN, AND, fMN, fLP);
    addrule(fMP, AND, fMN, fSN);
    addrule(fMP, AND, fMP, fLN);*/


}

int16_t transControlCurrent()
{
  return controlCurrent;
}

int16_t transOut()
{
  return (int16_t)currentNow;
}

/*** E_15 for Braking Set Direction        ***/
#define lineBrakeDirIN1     PAL_LINE( GPIOE, 15 )
#define lineBrakeDirIN2     PAL_LINE( GPIOG, 1 )

void setBrakeDirection ( bool direct )
{
    if ( direct )
    {
        palSetLine( lineBrakeDirIN1 );
        palClearLine( lineBrakeDirIN2 );
    }
    else
    {
        palClearLine( lineBrakeDirIN1 );
        palSetLine( lineBrakeDirIN2 );
    }
}

void brakeUnitCSSetPower( int16_t pressPower )
{
    tfconf.k = pressPower;
    setBrakeDirection( pressPower < 0 );
}

void PIControlInit ( PIControllerContext_t *ctx )
{
    ctx->err        = 0;
    ctx->prevErr    = 0;
    ctx->integrSum  = 0;
}


controllerRensponse_t PIControlResponse ( PIControllerContext_t *ctx )
{
    controllerRensponse_t control = 0;
//    if( (ctx->err < ctx->integZone_abs) && (ctx->err > -ctx->integZone_abs))
        ctx->integrSum += ctx->ki * ctx->err;
//    else
//        ctx->integrSum = 0;
    /* Symmetric limits */
    ctx->integrSum = constrain( ctx->integrSum, -ctx->integrLimit, ctx->integrLimit );

    control = ctx->kp * ctx->err +
                ctx->integrSum +
                ctx->kd * (ctx->err - ctx->prevErr);

    ctx->prevErr = ctx->err;
    return control;
}

