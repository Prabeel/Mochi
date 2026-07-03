#pragma once

#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver(int ain1, int ain2, int pwma,
                int bin1, int bin2, int pwmb,
                int stby);

    void begin();
    void stop();
    void forward(uint8_t speed);
    void back(uint8_t speed);
    void setMotorSpeeds(int left, int right);

private:
    int _ain1, _ain2, _pwma;
    int _bin1, _bin2, _pwmb;
    int _stby;
};