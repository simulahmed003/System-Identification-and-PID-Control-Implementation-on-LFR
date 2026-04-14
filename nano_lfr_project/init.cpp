#include <Arduino.h>
#include "config.h"
#include "init.h"

void startMux() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(sig, INPUT);
}

void startMotor() {
  pinMode(pwmR, OUTPUT);
  pinMode(pwmL, OUTPUT);
  pinMode(dirR, OUTPUT);
  pinMode(dirL, OUTPUT);

  analogWrite(pwmR, 0);
  analogWrite(pwmL, 0);
  digitalWrite(dirR, LOW);
  digitalWrite(dirL, LOW);
}

