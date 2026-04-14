#include <Arduino.h>
#include "config.h"
#include "functions.h"
#include "init.h"





void setup() {
  startMux();
  startMotor();
  pinMode(SW0, INPUT_PULLUP);

  initializePIDSettings();

  setting.numPID = PID_0;
  setting.lineColor = blackLine;
  setting.sensor_sensitivity = 50;
  setting.speed = 0;

  for (int i = 0; i < 16; i++) {
    setting.LIMIT_VALUE_SENSOR[i] = preDefinedThreshold[i];
  }


  for (int i = 0; i < 16; i++) setting.LIMIT_VALUE_SENSOR[i] = preDefinedThreshold[i];

  commsInit(9600);


  g_U = 50;
  g_Emax = 100;
  g_M = 1;      // stop
  g_S = 1;      // basic telemetry
  g_B = 0;    // base speed
  g_Mn = -50;     // min motor
  g_Mx = 99;   // max motor
  

}

void loop() {
  sensorBits = readSensor();

  if (g_M != 3 && g_M != 4) {
    lastError = errorA(sensorBits);
  }

  commsPoll();
  robotControlLoop();
  telemetryLoop();
  commsDebugPrintIfUpdated();
}
