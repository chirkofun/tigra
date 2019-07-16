#include <common.h>
#include <string.h>

/***********************/
/*** Common EXT unit ***/
/***********************/

/*
 * @brief       Counts encoder ticks
 * @param [in]  EXT driver, EXT channel
 */
void EXT_CB_A( EXTDriver *extp, expchannel_t channel )
{
    extp = extp;
    channel = channel;

    if(palReadLine(ENC_CH_B))
    {
        encoderTicks--;
    }
    else
    {
        encoderTicks++;
    }
}

/*
 * @brief       Allow parking mode
 * @param [in]  EXT driver, EXT channel
 */
void EXT_CB_PARK_ENABLE( EXTDriver *extp, expchannel_t channel )
{
    extp = extp;
    channel = channel;
    for(int i = 0; i<300000; i++);
    if(palReadPad(GPIOG,6) == PAL_HIGH)
    {
//        start_motor();
        turn_enable = true;
        drive_enable = true;
        parking_enable = true;
        palSetLine( LINE_LED2 );
    }
    else
    {
//        stop_motor();
        I = 0;
            turn_enable = false;
        drive_enable = false;
        parking_enable = false;
        palClearLine( LINE_LED2 );
    }
}

/************************/

/*** Common ADC1 unit ***/

/************************/



#define COMMON_ADC_CHANNELS_NUMBER  4

#define COMMON_ADC_BUFFER_DEPTH     4



static adcsample_t  adc_raw_buffer[COMMON_ADC_CHANNELS_NUMBER * COMMON_ADC_BUFFER_DEPTH];

static uint32_t     filtered_buffer[COMMON_ADC_CHANNELS_NUMBER];



static void adc_cb ( ADCDriver *adcp, adcsample_t *buffer, size_t n );



static const ADCConversionGroup  conv_group = {

    .circular       = true,

    .end_cb         = adc_cb,

    .error_cb       = NULL,

    .num_channels   = COMMON_ADC_CHANNELS_NUMBER,



    .cr1            = COMMON_ADC_RES_CONF,

    .cr2            = ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(0b1101),



    /* Manual configuration required */

    .smpr1          = ADC_SMPR1_SMP_AN10(ADC_SAMPLE_480) |

                      ADC_SMPR1_SMP_AN13(ADC_SAMPLE_480) |

                      ADC_SMPR1_SMP_AN12(ADC_SAMPLE_480),

    .smpr2          = ADC_SMPR2_SMP_AN3(ADC_SAMPLE_480),



    .sqr1           = ADC_SQR1_NUM_CH(COMMON_ADC_CHANNELS_NUMBER),

    .sqr2           = 0,

    .sqr3           = ADC_SQR3_SQ1_N(COMMON_ADC_SEQ1) |

                      ADC_SQR3_SQ2_N(COMMON_ADC_SEQ2) |

                      ADC_SQR3_SQ3_N(COMMON_ADC_SEQ3) |

                      ADC_SQR3_SQ4_N(COMMON_ADC_SEQ4)

};



static const GPTConfig trigger_cfg = {

    .frequency      =  1000000,         // 1 MHz

    .callback       =  transm,

    .cr2            =  TIM_CR2_MMS_1,

    .dier           =  0U

};



static ADCDriver                *commonADCDriver    = &ADCD1;

static GPTDriver                *adcTriggerDriver   = &GPTD6;



/* Set desired filter */

#define FILTER_MEAN         0

#define FILTER_LPF          1



#define ACTIVE_FILTER       FILTER_LPF



static void adc_cb ( ADCDriver *adcp, adcsample_t *buffer, size_t n )

{

    adcp = adcp; n = n;



    if ( buffer != adc_raw_buffer )

    {



#if (ACTIVE_FILTER == FILTER_MEAN)



        memset( filtered_buffer, 0, sizeof( filtered_buffer ) );



        for ( int ch = 0; ch < COMMON_ADC_CHANNELS_NUMBER; ch++ )

        {

            for ( int d = 0; d < COMMON_ADC_BUFFER_DEPTH; d++ )

            {

                filtered_buffer[ch] += adc_raw_buffer[d*COMMON_ADC_CHANNELS_NUMBER + ch];

            }



            filtered_buffer[ch] /= COMMON_ADC_BUFFER_DEPTH;

        }



#elif (ACTIVE_FILTER == FILTER_LPF)



        /* Don`t make any mistakes calculating rates and inversed */

        const float lpf_rates[COMMON_ADC_CHANNELS_NUMBER]     = {0.2, 0.1, 0.1, 0.1};

        const float lpf_rates_inv[COMMON_ADC_CHANNELS_NUMBER] = {0.8, 0.9, 0.9, 0.1};



        for ( int ch = 0; ch < COMMON_ADC_CHANNELS_NUMBER; ch++ )

        {

            for ( int d = 0; d < COMMON_ADC_BUFFER_DEPTH; d++ )

            {

                filtered_buffer[ch] = filtered_buffer[ch] * lpf_rates_inv[ch] +

                                        adc_raw_buffer[d*COMMON_ADC_CHANNELS_NUMBER + ch] * lpf_rates[ch];

            }

        }

#endif



    }

}



static bool     adcInitialized      = false;

static float    adcValue2Ref        = 0;

static float    referenceVoltage_mV = 3300;



void commonADC1UnitInit ( void )

{

    if ( adcInitialized )

        return;



    memset( filtered_buffer, 0, sizeof( filtered_buffer ) );



    adcStart( commonADCDriver, NULL );



    palSetLineMode( COMMON_ADC_SEQ1_LINE, PAL_MODE_INPUT_ANALOG );

    palSetLineMode( COMMON_ADC_SEQ2_LINE, PAL_MODE_INPUT_ANALOG );

    palSetLineMode( COMMON_ADC_SEQ3_LINE, PAL_MODE_INPUT_ANALOG );

    palSetLineMode( COMMON_ADC_SEQ4_LINE, PAL_MODE_INPUT_ANALOG );


    gptStart( adcTriggerDriver, &trigger_cfg );
    /* 2.5ms trigger for filter */

    adcStartConversion( commonADCDriver, &conv_group, adc_raw_buffer, COMMON_ADC_BUFFER_DEPTH );

    gptStartContinuous( adcTriggerDriver, 500 );
    //gptStartContinuous( adcTriggerDriver, trigger_cfg.frequency/100000 );


#if ( COMMON_ADC_RES_CONF == ADC_CR1_6B_RESOLUTION )
    adcValue2Ref            = referenceVoltage_mV / ((1 << 6) - 1);
#elif ( COMMON_ADC_RES_CONF == ADC_CR1_8B_RESOLUTION )
    adcValue2Ref            = referenceVoltage_mV / ((1 << 8) - 1);
#elif ( COMMON_ADC_RES_CONF == ADC_CR1_10B_RESOLUTION )
    adcValue2Ref            = referenceVoltage_mV / ((1 << 10) - 1);
#else
    adcValue2Ref            = referenceVoltage_mV / ((1 << 12) - 1);
#endif

    adcInitialized = true;
}

adcsample_t commonADC1UnitGetValue ( uint8_t ch )
{
    if ( ch >= COMMON_ADC_CHANNELS_NUMBER )
    {
        return 0;
    }
    return filtered_buffer[ch];
}
adc1SampleMV_t commonADC1UnitGetValueMV ( uint8_t ch )

{
    if ( ch >= COMMON_ADC_CHANNELS_NUMBER )
    {
        return 0;
    }

    return filtered_buffer[ch] * adcValue2Ref;
}

