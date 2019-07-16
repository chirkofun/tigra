/*
 * drive_speed.c
 *
 *  Created on: 17 мая 2019 г.
 *      Author: cyirkofun
 */
/*
 * Definition of port and pad for drive speed control
 */
#include <drive_speed.h>

#define driveSpeedLine          PAL_LINE( GPIOA, 4 )

static  DACDriver       *dacDriver      = &DACD1;

/*
 * Transform voltage to DAC
 */
#define                         REF_V 3.125
#define                         DAC_MAX 4095
#define                         VOLTAGE_2_DAC(v)  ((v) / REF_V * DAC_MAX)

/*
 * DAC config for speed control
 */
static const DACConfig DAC1cfg = {
  .init         = 0,
  .datamode     = DAC_DHRM_12BIT_RIGHT,
  .cr           = 0
};

/*****************************************************************************/

/*
 * Definition of line for drive direction control
 */
#define driveDirectionLine      PAL_LINE( GPIOD, 3 )

/*****************************************************************************/

static bool         isInitialized       = false;
bool drive_enable = false;

/*
 * @brief   Initialization of drive
 */
void drive_Speed_Init(void)
{
    if ( isInitialized ) return;

    palSetLineMode(driveSpeedLine, PAL_MODE_INPUT_ANALOG);          // GPIOA 4 for DAC
    palSetLineMode(driveDirectionLine, PAL_MODE_OUTPUT_OPENDRAIN ); // GPIOD 3 for direction
    dacStart(&DACD1, &DAC1cfg);                                     // DAC driver start

    isInitialized = true;
}

/*
 * @brief   Set motor direction
 * @param   driveDirection true - forward
 *                         false - backward
 */
void drive_Direction_Control ( bool driveDirection )
{
    if( driveDirection )
        palSetLine( driveDirectionLine );
    else
        palClearLine( driveDirectionLine );
}

/*
 * @brief   Set motor direction
 * @param   voltage 3V - max speed
 *                  0V - no speed
 */
void drive_Speed_Control( float voltage )
{
    if(!drive_enable) return;
    uint16_t dacValue = VOLTAGE_2_DAC(voltage);
    dacPutChannelX( dacDriver , 0 , dacValue );
}
