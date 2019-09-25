//
// Created by Hwiyong on 2019-08-21.
//

#include <stdio.h>
#include "dc_motor.h"
#include "ultrasonic.h"

// AutoDrive variable
#define ACTIVE_TIME 1 // minute
#define SAFE_DISTANCE 50 // centimeter

// dc_motor
#define MOTOR_COUNT 2
#define INIT_SPEED 50
#define TURN_SPEED 80

// ultrasonic
#define ULTRASONIC_UP_FORWARD_TRIG 27 // BCM 16
#define ULTRASONIC_UP_FROWARD_ECHO 23 // BCM 13
#define ULTRASONIC_DOWN_FORWARD_TRIG 29	// BCM 21
#define ULTRASONIC_DOWN_FORWARD_ECHO 28	// BCM 20

// fan
#define RELAY_PIN 24 // BCM 19
#define FAN_ACTIVE_SECOND 10 // second

// functions
int initAllModule();

void autoDrive(int minute, int safeDistance);

void goForward();
void goBack();
void turnLeft(int angle);
void turnRight(int angle);
void stop();

int getShortUltrasonicDistance();

void fanOn();
void fanOff();

// fields
int motor[MOTOR_COUNT];
int speed = INIT_SPEED;

int main() {
    if(initAllModule() == 0) {
        printf("Auto driving Start~\n");
        autoDrive(ACTIVE_TIME, SAFE_DISTANCE);
        printf("Auto driving End!\n");
    }
    stop();
    fanOff();
    return 0;
}

int initAllModule() {
    if(wiringPiSetup() == -1)
        return -1;

    // DC motor
    initI2C();
    for(int i=0; i<MOTOR_COUNT; ++i)
        motor[i] = i+1;

    // ultrasonic
    initUltrasonic(ULTRASONIC_UP_FORWARD_TRIG, ULTRASONIC_UP_FROWARD_ECHO);
    initUltrasonic(ULTRASONIC_DOWN_FORWARD_TRIG, ULTRASONIC_DOWN_FORWARD_ECHO);

    // fan
    pinMode(RELAY_PIN, OUTPUT);
    fanOff();

    return 0;
}

void autoDrive(int minute, int safeDistance) {
    int moveMilliSecond = 0;
    int leftMilliSecond = minute * 60 * 1000;
    int shortDistance = getShortUltrasonicDistance();
    while(leftMilliSecond > 0) {
        if(moveMilliSecond > FAN_ACTIVE_SECOND * 1000) {
            stop();
            fanOn();
            sleep(FAN_ACTIVE_SECOND);
            fanOff();
            moveMilliSecond -= FAN_ACTIVE_SECOND * 1000;
        }
        else if(shortDistance > safeDistance) {
            goForward();
            usleep(100 * 1000);	// temp second
            leftMilliSecond -= 100;
            moveMilliSecond += 100;
        }
        else {
            turnLeft(45);
            leftMilliSecond -= 27;
            moveMilliSecond += 27;
        }
        shortDistance = getShortUltrasonicDistance();
    }
}

void goForward() {
    int i;
    for(i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
    for(i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_FORWARD);
}

void goBack() {
    int i;
    for(i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
    for(i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_BACK);
}

void turnLeft(int angle) {
    int i;
    for(i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], TURN_SPEED);
    runDCMotor(motor[1], MOTOR_FORWARD);
    runDCMotor(motor[0], MOTOR_BACK);
    usleep(angle * 1 * 300);
    for(i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
}

void turnRight(int angle) {
    int i;
    for(i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], TURN_SPEED);
    runDCMotor(motor[0], MOTOR_FORWARD);
    runDCMotor(motor[1], MOTOR_BACK);
    usleep(angle * 1 * 300);
    for(int i=0; i<MOTOR_COUNT; ++i)
        setDCSpeed(motor[i], speed);
}

void stop() {
    int i;
    for(i=0; i<MOTOR_COUNT; ++i)
        runDCMotor(motor[i], MOTOR_RELEASE);
}

int getShortUltrasonicDistance() {
    int upUltrasonicDistance = getUltrasonicDistance(ULTRASONIC_UP_FORWARD_TRIG, ULTRASONIC_UP_FROWARD_ECHO);
    int downUltrasonicDistance = getUltrasonicDistance(ULTRASONIC_DOWN_FORWARD_TRIG, ULTRASONIC_DOWN_FORWARD_ECHO);
    if(upUltrasonicDistance > 1000 || downUltrasonicDistance > 1000)
        return 0;
    return upUltrasonicDistance < downUltrasonicDistance ? upUltrasonicDistance : downUltrasonicDistance;
}

void fanOn() {
    digitalWrite(RELAY_PIN, HIGH);
}

void fanOff() {
    digitalWrite(RELAY_PIN, LOW);
}