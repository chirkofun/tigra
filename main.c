#include <drive_speed.h>
#include <enc.h>
#include <odometry.h>
#include <sonar_sensors.h>
#include <common.h>
#include <init_ADC.h>
#include <motCtrl.h>
#include <tf_calculation.h>
#include <steering_wheel.h>

#define MIN_DIST 3.8
#define WHEEL_BASE 1.24
#define ARC_LENGTH 2.08

#define stopLight           PAL_LINE( GPIOE, 2 )



bool parking_enable = false;

int main(void)
{

    chSysInit();
    halInit();
    lld_Odometry_Init();

    drive_Speed_Init();
    lldMotorInit();

    initADC();
    serial_Init();
    initialization();
    Fuzzy_Init();
    brakeSensorInit();

    uint16_t distanceLeftSonar = 0, placeWidth = 0;
    float placeLength = 0, nowDist = 0;
    float robotSpeed = 0;
    bool searchingPlace = true, parking1 = false, parking2 = false;
    int gov = 0;
    int t =0;
    uint16_t firstDistanceLeftSonar = 40;
    int graf = 0;

//
//    chprintf(((BaseSequentialStream *)&SD7), "%d    ",turn_enable);
//    chprintf(((BaseSequentialStream *)&SD7), "%d    ",drive_enable);
//    chprintf(((BaseSequentialStream *)&SD7), "%d    \r\n",parking_enable);
//

    steerUnitCSSetPosition(0);
    chThdSleepSeconds(2);

    while (true)
    {
//        drive_Direction_Control(true);
//        chThdSleepMilliseconds(41);
//        drive_Speed_Control(1.35);
//        chThdSleepSeconds(3);
//        drive_Speed_Control(0);
//
//        brakeUnitCSSetPower(100);
//        chThdSleepMilliseconds(1000);
//        brakeUnitCSSetPower(-50);
//        chThdSleepSeconds(1);
//
//        drive_Direction_Control(false);
//        chThdSleepMilliseconds(41);
//        drive_Speed_Control(1.35);
//        chThdSleepSeconds(3);
//        drive_Speed_Control(0);
//
//        brakeUnitCSSetPower(100);
//        chThdSleepMilliseconds(1000);
//        brakeUnitCSSetPower(-50);
//        chThdSleepSeconds(1);
//        gov = get_wheel_pos();
//        t = commonADC1UnitGetValue(1);
//        chprintf(((BaseSequentialStream *)&SD7), "con:%d\n\r",brakeSensorIsPressed());
//        chprintf(((BaseSequentialStream *)&SD7), "%d    ",commonADC1UnitGetValue(1));
//        chprintf(((BaseSequentialStream *)&SD7), "%d    ",f);
//        chprintf(((BaseSequentialStream *)&SD7), "%d    ",turn_enable);
//        chprintf(((BaseSequentialStream *)&SD7), "%d    ",gov);
//        distanceLeftSonar = get_sonar_dist(leftSonar, 4);
        nowDist = get_distance();
        robotSpeed = get_speed();
        nowDist *= 100;
        robotSpeed *= 100;
//       chprintf(((BaseSequentialStream *)&SD7), "%d  ",distanceLeftSonar);
        chprintf(((BaseSequentialStream *)&SD7), "%d  ",(int)nowDist);
        chprintf(((BaseSequentialStream *)&SD7), "%d  \n\r",(int)robotSpeed);
//        chprintf(((BaseSequentialStream *)&SD7), "con:%d\n\r",brakeSensorIsPressed());
//
        if(parking_enable)
        {
                    if(searchingPlace)
                    {
                        drive_Direction_Control(true);
                        chThdSleepMilliseconds(41);
                        drive_Speed_Control(1.35);
                        distanceLeftSonar = get_sonar_dist(leftSonar, 4);
                        placeWidth = distanceLeftSonar - firstDistanceLeftSonar;
                        if(placeWidth < 150)
                        {
                            encoderTicks = 0;
                            currentDistance = 0;
                            prevDistance = 0;
                        }
                        else
                        {
                            placeLength = get_distance();
                            robotSpeed = get_speed();
                            if(placeLength > MIN_DIST)
                            {
                                drive_Speed_Control(0);

                                if(((get_speed() > 0.3)||(get_speed() < -0.3)) && gov == 0)
                                {
                                    brakeUnitCSSetPower(100);
                                    chThdSleepMilliseconds(2000);
                                    brakeUnitCSSetPower(-50);
                                    gov = 1;
                                }

                                encoderTicks = 0;
                                currentDistance = 0;
                                prevDistance = 0;

                                chThdSleepSeconds(3);
                                searchingPlace = false;
                                parking1 = true;

                            }
                        }
                    }

                    if(parking1)
                    {
                        steerUnitCSSetPosition(-100);
                        nowDist = get_distance();
                        drive_Direction_Control(false);
                        chThdSleepMilliseconds(41);

                        if(nowDist > -ARC_LENGTH)
                        {
                            drive_Speed_Control(1.35);
                        }
                        else
                        {
                            drive_Speed_Control(0);
                            if(((get_speed() > 0.3)||(get_speed() < -0.3)) && gov == 0)
                            {
                                brakeUnitCSSetPower(100);
                                chThdSleepMilliseconds(1000);
                                brakeUnitCSSetPower(-50);
                                gov = 1;
                            }

                            encoderTicks = 0;
                            currentDistance = 0;
                            prevDistance = 0;


                            chThdSleepSeconds(3);
                            parking1 = false;
                            parking2 = true;
                        }

                    }

                    if(parking2)
                    {
                        if(graf == 0)
                        {
                            encoderTicks = 0;
                                                        currentDistance = 0;
                                                        prevDistance = 0;
                                                        graf = 1;
                        }
                        steerUnitCSSetPosition(100);
                        nowDist = get_distance();

                        drive_Direction_Control(false);
                        chThdSleepMilliseconds(41);

                        if(nowDist > -ARC_LENGTH)
                        {
                            drive_Speed_Control(1.35);
                        }
                        else
                        {

                            drive_Speed_Control(0);
                            if(((get_speed() > 0.3)||(get_speed() < -0.3)) && gov == 0)
                            {
                                brakeUnitCSSetPower(100);
                                chThdSleepMilliseconds(1000);
                                brakeUnitCSSetPower(-50);
                                gov = 1;
                            }

                            encoderTicks = 0;
                            currentDistance = 0;
                            prevDistance = 0;
                            nowDist = 0;

                            chThdSleepSeconds(3);
                            parking2 = false;

                        }
                    }
        }
        chThdSleepMilliseconds(100);
    }
}
