//
// Created by Hwiyong on 2019-08-21.
//

#ifndef SMARTMOBILEPURIFIER_RASPBERRYPI3BPLUS_ULTRASONIC_H
#define SMARTMOBILEPURIFIER_RASPBERRYPI3BPLUS_ULTRASONIC_H

#include <wiringPi.h>
#include <unistd.h>

void initUltrasonic(int triggerPin, int echoPin);
int getUltrasonicDistance(int triggerPin, int echoPin);

#endif //SMARTMOBILEPURIFIER_RASPBERRYPI3BPLUS_ULTRASONIC_H
