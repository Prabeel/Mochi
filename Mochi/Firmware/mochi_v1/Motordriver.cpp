#include "MotorDriver.h"

MotorDriver::MotorDriver(int ain1, int ain2, int pwma,
                         int bin1, int bin2, int pwmb,
                         int stby) {
    _ain1 = ain1;
    _ain2 = ain2;
    _pwma = pwma;
    _bin1 = bin1;
    _bin2 = bin2;
    _pwmb = pwmb;
    _stby = stby;
}

void MotorDriver::begin() {
    Serial.println("MotorDriver simulation started");
}

void MotorDriver::setMotorSpeeds(int left, int right) {
    Serial.print("LEFT: ");
    Serial.print(left);

    Serial.print(" | RIGHT: ");
    Serial.println(right);
}

void MotorDriver::forward(uint8_t speed) {
    setMotorSpeeds(speed, speed);
}

void MotorDriver::back(uint8_t speed) {
    setMotorSpeeds(-speed, -speed);
}

void MotorDriver::stop() {
    setMotorSpeeds(0, 0);
}