/*
 * odometry.h
 *
 *  Created on: 22 февр. 2019 г.
 *      Author: cyirkofun
 */

#ifndef ODOMETRY_H_
#define ODOMETRY_H_

#include <enc.h>

typedef float odometryDistanceValue_t;
typedef float odometrySpeedValue_t;

/* Distance, Previous distance, changing the distance*/
extern float currentDistance, prevDistance,  diffDistance;

/*
 * @brief       Counts distance traveled (for certain wheel radius and transmition ratio)
 * @return      Distance in meters
 */
odometryDistanceValue_t get_distance (void);

/*
 * @brief       Odometry initialization and start
 */
void lld_Odometry_Init (void);

/*
 * @brief       Counts moving speed: distance changing divides by time changing
 * @return      Speed in meters/seconds
 */
odometrySpeedValue_t get_speed (void);


#endif /* ODOMETRY_H_ */
