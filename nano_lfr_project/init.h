#ifndef INIT_H
#define INIT_H

#include <Arduino.h>

/*
  Hardware init only.
  Keep protocol/logic out of init:
  - UART protocol init is in functions_uart.cpp (commsInit)
*/

void startMux();
void startMotor();


#endif // INIT_H
