/*
 * drive_speed.h
 *
 *  Created on: 17 мая 2019 г.
 *      Author: cyirkofun
 */

#ifndef SRC_DRIVE_SPEED_H_
#define SRC_DRIVE_SPEED_H_

#include <common.h>



/*
 * @brief   Initialization of drive
 */
void drive_Speed_Init(void);

/*
 * @brief   Set motor direction
 * @param   driveDirection true - forward
 *                         false - backward
 */
void drive_Direction_Control ( bool driveDirection );

/*
 * @brief   Set motor direction
 * @param   voltage 3V - max speed
 *                  0V - no speed
 */
void drive_Speed_Control( float voltage );


#endif /* SRC_DRIVE_SPEED_H_ */
