/*
 * enc.c
 *
 *  Created on: 21 февр. 2019 г.
 *      Author: cyirkofun
 */

#include <enc.h>

/*Encoder maximum number of ticks*/
#define ENC_MAX_TICK 360

/*Encoder initialization flag */
static bool Enc_is_Initialized = false;

int32_t encoderTicks = 0;

/*
 * @brief       Starts EXT module
 */
void lld_Encoder_Init(void)
{
    if ( Enc_is_Initialized ) return;
    extStart( &EXTD1, &EXTcfg );
    Enc_is_Initialized = true;
}

/*
 * @brief       Return encoder ticks
 * @return      Encoder ticks
 */
encoderTicksValue_t get_ticks(void)
{
    return(encoderTicks);
}

/*
 * @brief       Counts encoder turnovers
 * @return      Number of turnovers
 */
encoderTurnoverValue_t get_turnover(void)
{
    return((float)encoderTicks/ENC_MAX_TICK);
}






