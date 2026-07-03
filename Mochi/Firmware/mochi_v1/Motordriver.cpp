#include "MotorDriver.h"

  MotorDriver::MotorDriver(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb, int stby) {
    _ain1 = ain1;
    _ain2 = ain2;
    _pwma = pwma;
    _bin1 = bin1;
    _bin2 = bin2;
    _pwmb = pwmb;
    _stby = stby;
  }

  void MotorDriver::begin() {
    pinMode(_ain1, OUTPUT);
    pinMode(_ain2, OUTPUT);
    pinMode(_pwma, OUTPUT);
    pinMode(_bin1, OUTPUT);
    pinMode(_bin2, OUTPUT);
    pinMode(_pwmb, OUTPUT);
    pinMode(_stby, OUTPUT);
    stop(); 
  }

  void MotorDriver::setMotorSpeeds(int left, int right){
    digitalWrite(_stby, HIGH);

    if(left >= 0){
      digitalWrite(_ain1,HIGH);
      digitalWrite(_ain2,LOW);
    }else{
      digitalWrite(_ain1,LOW);
      digitalWrite(_ain2,HIGH);
      left = -left;
    }

    if(right >= 0){
      digitalWrite(_bin1,HIGH);
      digitalWrite(_bin2,LOW);
    }else{
      digitalWrite(_bin1,LOW);
      digitalWrite(_bin2,HIGH);
      right = -right;
    }
  }

  void MotorDriver::forward(uint8_t speed) {
    setMotorSpeeds(speed,speed);
  }

  void MotorDriver::back(uint8_t speed) {
    setMotorSpeeds(-speed,-speed);
  }

  void MotorDriver::stop() {
    setMotorSpeeds(0,0);
    digitalWrite(_stby, LOW); 
  }