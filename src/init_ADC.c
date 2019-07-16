#include <init_ADC.h>

//static adcsample_t samples1[ADC1_NUM_CHANNELS * ADC1_BUF_DEPTH];


static int16_t n = 15, count = 0, count2 = 0, zero = 2048, window = 5;
static int16_t inp[3000];
static int16_t out[3000];
static int16_t retSpeed=0;
static int16_t outSpd = 0;
static int16_t PWMFiltrationCounter = 0;
static int32_t PWMAverage = 0;
static int16_t countt = 0;
int16_t fuzzyCurrent;



int getADC()
{
  return fuzzyCurrent;
}

void smooth(int16_t *input, int16_t *output, int n, int window)
{
   int q,i,j,z,k1,k2,hw;
   double tmp, average;
   if(window % 2==0) window++;
   hw=(window-1)/2;
   output[0]=input[0];


   for (i=1;i<n;i++)
   {
       tmp=0;
       if(i<hw){
           k1=0;
           k2=2*i;
           z=k2+1;
       }
       else if((i+hw)>(n-1)){
           k1=i-n+i+1;
           k2=n-1;
           z=k2-k1+1;
       }
       else{
           k1=i-hw;
           k2=i+hw;
           z=window;
       }

       for (j=k1;j<=k2;j++){
           tmp=tmp+input[j];
       }
       if(i % 30 == 0)
       {
         for(q=i;q<i-30;q++)
         {
           average += output[q];
         }
         average/=30;
       }
       if(tmp/z > average + average*0.01)
       {
         output[i]=average;
       }
       else
       {
         output[i]=tmp/z;
       }
   }
}

void transm ()
{
  PWMAverage += commonADC1UnitGetValue(0) & 0b1111111111110000; //0xfffc
  PWMFiltrationCounter++;
  if (PWMFiltrationCounter == 20)
  {
      PWMAverage /= 20;
      PWMFiltrationCounter = 0;
      BrakeFunc();

      fuzzyCurrent = PWMAverage;
      retSpeed = transOut();
      //outSpd = transControlCurrent();
      //sdWrite(&SD7, &retSpeed, 2);
      //sdWrite(&SD7, &outSpd, 2);

      PWMAverage = 0;
      //inp[count]=PWMAverage;
      //count++;
  }

  /*
    if (count > n)
    {
      smooth(&inp, &out, n, window);
    }
    if(count2 < n)
    {
      /*retSpeed=out[count2];
      sdWrite(&SD7, &retSpeed, 2);
      count2++;
    }
    else
    {
      count = 0;
      count2 = 0;
    }*/
  /*if (count2 < n)
  {

   DM
        fuzzyCurrent = out[count2];
        retSpeed = inp[count2];

        fuzzyCurrent = out[count2];
        retSpeed =transOut();
        outSpd = getSpd();
        sdWrite(&SD7, &retSpeed, 2);
        sdWrite(&SD7, &outSpd, 2);

         DM
        sdWrite(&SD7, &fuzzyCurrent, 2);
        sdWrite(&SD7, &retSpeed, 2);

        count2++;
  }*/
}

//static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
//{
//  (void)adcp;
//  (void)buffer;
//  (void)n;
//}

///* ADC errors callback, should never happen */
//static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {
//
//  (void)adcp;
//  (void)err;
//}

//static const ADCConversionGroup adcgrpcfg1 = {
//  .circular     = true,                                           // working mode = looped
//  .num_channels = ADC1_NUM_CHANNELS,
//  .end_cb       = adccallback,
//  .error_cb     = adcerrorcallback,
//  .cr1          = 0,
//  .cr2          = ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(0b1100),  // Commutated from GPT
//  .smpr1        = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144),             // for AN10 - 144 samples
//  .smpr2        = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_144),              // for AN3  - 144 samples
//  .sqr1         = ADC_SQR1_NUM_CH(ADC1_NUM_CHANNELS),
//  .sqr2         = 0,
//  .sqr3         = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3) |               // sequence of channels
//                  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10)
//  /* If we can macro ADC_SQR2_SQ... we need to write to .sqr2 */
//};

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

PWMConfig pwm2conf = {
    .frequency = 150000,
    .period    = 10,
    .callback  = NULL,
    .channels  = {
                  {.mode = PWM_OUTPUT_DISABLED, .callback = NULL}, // Channel 1 is working CH1 = PE9
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    .cr2        = 0,
    .dier       = 0
};


void initialization(void)
{
      palSetPadMode(GPIOE, 15,PAL_MODE_OUTPUT_PUSHPULL);
      palSetPadMode(GPIOG, 1,PAL_MODE_OUTPUT_PUSHPULL);

      palSetLineMode( PAL_LINE( GPIOB, 10 ),  PAL_MODE_ALTERNATE(1) );
      pwmStart( &PWMD2, &pwm2conf );
}



