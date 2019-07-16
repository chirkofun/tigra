/*
 * odometry.c
 *
 *  Created on: 22 февр. 2019 г.
 *      Author: cyirkofun
 */

#include <odometry.h>

/* Wheel radius */
#define RADIUS 0.26
/* Reducer transmition ratio  */
#define TRANSMITION_RATIO 0.125

/*Odometry initialization flag */
static bool Odometry_is_Initialized = false;
float currentDistance = 0, prevDistance = 0,  diffDistance = 0;

/*
 * @brief       Counts distance traveled (for certain wheel radius and transmition ratio)
 * @return      Distance in meters
 */
odometryDistanceValue_t get_distance (void)
{
    float distance = get_turnover()*(2*M_PI*RADIUS)*TRANSMITION_RATIO;
    return(distance);
}

/*
 * @brief       GPT Callback: counts previos distance and changing the distance
 */
static void GPT_CB( GPTDriver *gptd )
{
    gptd = gptd;
    prevDistance = currentDistance;
    currentDistance = get_distance();
    diffDistance = currentDistance - prevDistance;
}

/* GPT1 driver assignment*/
static GPTDriver    *GPT_Driver     = &GPTD9;

/* GPT1 driver config*/
GPTConfig GPT9cfg = {
    .frequency    = 10000,
    .callback     = GPT_CB,
    .cr2          = 0,
    .dier         = 0
};

#define TMR_TICKS_2_OVRFLOW   (GPT9cfg.frequency/100)

/*
 * @brief       Odometry initialization and start
 */
void lld_Odometry_Init (void)
{
    if( Odometry_is_Initialized ) return;
    gptStart( GPT_Driver, &GPT9cfg );
    gptStartContinuous( GPT_Driver, TMR_TICKS_2_OVRFLOW );
    lld_Encoder_Init();
    Odometry_is_Initialized = true;
}

/*
 * @brief       Counts moving speed: distance changing divides by time changing
 * @return      Speed in meters/seconds
 */
odometrySpeedValue_t get_speed (void)
{
    float delta_time = (float)GPT9cfg.frequency/(TMR_TICKS_2_OVRFLOW);
    float speed = diffDistance*delta_time;
    return(speed);
}

