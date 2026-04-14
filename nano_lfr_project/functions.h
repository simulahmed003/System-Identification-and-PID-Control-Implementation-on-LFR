#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#define whiteLine 1
#define blackLine 0

extern SoftwareSerial SoftSerial;

extern int g_L;
extern int g_R;
extern uint8_t g_M;
extern uint8_t g_S;

extern float g_P;
extern float g_I;
extern float g_D;

extern int g_B;
extern int g_Mn;
extern int g_Mx;
extern int g_Sn;
extern int g_U;
extern int16_t g_Emax;

extern bool g_newFrame;

const uint32_t TELEMETRY_INTERVAL_MS = 50;

typedef struct {
  uint16_t LIMIT_VALUE_SENSOR[16];
  int speed;
  byte sensor_sensitivity;
  byte lineColor;
  byte numPID;
} dataSetting;

extern dataSetting setting;
extern uint16_t reading[16];
extern bool previouslyCalibrated;
extern uint16_t sensorBits;
extern uint16_t preDefinedThreshold[16];

typedef struct {
  float Kp;
  float Kd;
  float Ki;
  int PMax;
  int PMin;
} dataPID;

#define NUM_PID 4
#define PID_0 0
#define PID_1 1
#define PID_2 2
#define PID_3 3

extern dataPID listPID[NUM_PID];
extern double P;
extern double D;
extern double I;
extern int16_t error;
extern int16_t lastError;
extern unsigned long lastProcess;
extern bool validSensor[16];

void commsInit(uint32_t baud);
void controlVarsResetToDefault();
void commsPoll();
void commsDebugPrintIfUpdated();
void telemetryLoop();

void robotControlLoop();
void stepResponseControl();

uint16_t readSensor();
void setSensorSensivity(byte sensitivity);
void calibrateSensor();
void calibrationModeLoop();
int16_t errorD(int dataSensor);
int16_t errorA(uint16_t bits);

void initializePIDSettings();
void setPID(byte num, float Kp, float Ki, float Kd, byte PMax, int PMin);
void PID();
void setMotor(int16_t L, int16_t R);

#endif
