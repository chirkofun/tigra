/*
 * sonar_sensors.h
 *
 *  Created on: 20 мая 2019 г.
 *      Author: cyirkofun
 */

#ifndef INCLUDE_SONAR_SENSORS_H_
#define INCLUDE_SONAR_SENSORS_H_

#include <common.h>

#define  frontLeftSonar   &SD7
#define  leftSonar        &SD6
#define  backLeftSonar    &SD5

/*
 * Serials UART Initialization for sonar sensors.
 */
void serial_Init(void);

/*
 * @brief Bubble sorting function
 * @param x      - array
 *        window - size of array
 */
void Sort_Func (uint16_t *x, uint16_t window);

/*
 * @brief Choose medium value from array
 * @param x      - array
 *        window - size of array
 */
uint16_t Filter_Mediana (uint16_t *x, uint16_t window);

/*
 * @brief Bring distance from sonar sensor
 * @param   type_sonar  - selected sensor
 *          digits_num  - digits capacity (depending on type of sonar)
 *          window      - window for median filter
 */
uint16_t get_sonar_dist(SerialDriver *type_sonar, uint16_t digits_num );

#endif /* INCLUDE_SONAR_SENSORS_H_ */
