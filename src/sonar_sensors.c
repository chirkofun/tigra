#include <sonar_sensors.h>

//Serial 7 TX and RX ports
#define frontLeftSonarTX        PAL_LINE( GPIOE, 8 )
#define frontLeftSonarRX        PAL_LINE( GPIOE, 7 )

//Serial 6 TX and RX ports
#define leftSonarTX             PAL_LINE( GPIOC, 6 )
#define leftSonarRX             PAL_LINE( GPIOC, 7 )

//Serial 7 TX and RX ports
#define backLeftSonarTX         PAL_LINE( GPIOB, 13 )
#define backLeftSonarRX         PAL_LINE( GPIOB, 12 )

//Window fo Median Filter
#define WINDOW 10

bool isInitialized = false, firstValues = false;

uint32_t maxSonarSensorValue = 0;

uint16_t filterWindow[WINDOW], num, flt_num;

/*
 * Serial UART configuration.
 */
static const SerialConfig UARTcfg = {
        .speed  = 9600,
        .cr1    = 0,
        .cr2    = 0,
        .cr3    = 0
    };
/*
 * Serials UART Initialization for sonar sensors.
 */
void serial_Init(void)
{
    if(isInitialized) return;
    sdStart( frontLeftSonar, &UARTcfg );
    sdStart( leftSonar     , &UARTcfg );
    sdStart( backLeftSonar , &UARTcfg );

    palSetLineMode( frontLeftSonarTX, PAL_MODE_ALTERNATE(8) );      // TX for front left sensor
    palSetLineMode( frontLeftSonarRX, PAL_MODE_ALTERNATE(8) );      // RX for front left sensor
    palSetLineMode( leftSonarTX, PAL_MODE_ALTERNATE(8) );           // TX for left sensor
    palSetLineMode( leftSonarRX, PAL_MODE_ALTERNATE(8) );           // RX for left sensor
    palSetLineMode( backLeftSonarTX, PAL_MODE_ALTERNATE(8) );       // TX for back left sensor
    palSetLineMode( backLeftSonarRX, PAL_MODE_ALTERNATE(8) );       // RX for back left sensor

    palSetPad(GPIOF, 4); //High level pin 4 for Sonar front left
    palSetPad(GPIOF, 5); //High level pin 4 for Sonar left
    palSetPad(GPIOC, 4); //High level pin 4 for Sonar back left

    isInitialized = true;
}

/*
 * @brief Bubble sorting function
 * @param x      - array
 *        window - size of array
 */
void Sort_Func (uint16_t *x, uint16_t w)
{
    uint16_t buff = 0;
    for (uint16_t i=0; i<(w-1); i++ )
    {
        for (uint16_t j=0; j<(w-i-1); j++)
        {
            if (x[j]<x[j+1])
            {
                buff = x[j];
                x[j] = x[j + 1];
                x[j + 1] = buff;
            }
        }
    }
}

/*
 * @brief Choose medium value from array
 * @param x      - array
 *        window - size of array
 */
uint16_t Filter_Mediana (uint16_t *x, uint16_t w)
{
    uint16_t z[w];
    uint16_t y;

    for (unsigned int k=0; k<w; k++)
    {
        z[k]=x[k];
    }
    Sort_Func(z, w);
    y = z[w/2];
    return(y);
}

/*
 * @brief Bring distance from sonar sensor
 * @param   type_sonar  - selected sensor
 *          digits_num  - digits capacity (depending on type of sonar)
 *          window      - window for median filter
 */
uint16_t get_sonar_dist(SerialDriver *type_sonar, uint16_t digits_num )
{


    if(firstValues == false)
    {
        for (int i=0; i<WINDOW; i++)
        {
            maxSonarSensorValue = sdGet(type_sonar);
            if (maxSonarSensorValue == 'R')
            {
                sdRead(type_sonar, (uint8_t *)&maxSonarSensorValue , digits_num);
                filterWindow[i] = atoi(&maxSonarSensorValue);
                chThdSleepMilliseconds(30);
            }
            else
            {
                i--;
            }
        }
        firstValues = true;
    }

    maxSonarSensorValue = sdGet(type_sonar);
        if (maxSonarSensorValue == 'R')
        {
            flt_num = Filter_Mediana(filterWindow, WINDOW);
            for (int i=0; i<WINDOW-1; i++)
            {
                filterWindow[i] = filterWindow[i+1];
            }
            sdRead(type_sonar, (uint8_t *)&maxSonarSensorValue , digits_num);
            filterWindow[WINDOW-1] = atoi(&maxSonarSensorValue);
            chThdSleepMilliseconds(30);

            return(flt_num);
        }

}
