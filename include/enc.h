/*
 * enc.h
 *
 *  Created on: 21 февр. 2019 г.
 *      Author: cyirkofun
 */

#ifndef ENC_H_
#define ENC_H_

#include <common.h>
#include <odometry.h>

/* Encoder tick count, global value*/
extern int32_t encoderTicks;

typedef int32_t encoderTicksValue_t;
typedef float encoderTurnoverValue_t;

///*
// * @brief       Counts encoder ticks
// * @param [in]  EXT driver, EXT channel
// */
//void EXT_CB_A( EXTDriver *extp, expchannel_t channel );

/*
 * @brief       Starts EXT module
 */
void lld_Encoder_Init(void);

/*
 * @brief       Return encoder ticks
 * @return      Encoder ticks
 */
encoderTicksValue_t get_ticks(void);

/*
 * @brief       Counts encoder turnovers
 * @return      Number of turnovers
 */
encoderTurnoverValue_t get_turnover(void);


#endif /* ENC_H_ */
