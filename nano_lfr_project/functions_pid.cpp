#include <Arduino.h>
#include "config.h"
#include "functions.h"

// ===================== PID GLOBALS (defined once) =====================
dataPID listPID[NUM_PID];

double P = 0;
double D = 0;
double I = 0;

int16_t error = 0;
int16_t lastError = 0;

unsigned long lastProcess = 0;

// ---------------- initializePIDSettings()  ----------------
void initializePIDSettings() {
  setPID(PID_0, 6, 0, 35, 100, -50);
  setPID(PID_1, 8, 0, 40, 100, -50);
  setPID(PID_2, 9, 0, 45, 100, -50);
  setPID(PID_3, 10, 0, 25, 100, -50);
}

void setPID(byte num, float Kp, float Ki, float Kd, byte PMax, int PMin) {
  if (num < NUM_PID) {
    listPID[num].Kp = Kp;
    listPID[num].Ki = Ki;
    listPID[num].Kd = Kd;
    listPID[num].PMax = PMax;
    listPID[num].PMin = PMin;
  }
}

void PID() {
  double deltaTime = (millis() - lastProcess) / 1000.0;
  lastProcess = millis();
  if (deltaTime <= 0.0) deltaTime = 0.001;

  error = errorA(sensorBits);

  P = error * (double) listPID[setting.numPID].Kp;
  D = (error - lastError) * (double) listPID[setting.numPID].Kd / deltaTime;
  
  I += error * (double)listPID[setting.numPID].Ki * deltaTime;      

  lastError = error;
  int moveVal = (int)P + (int)D; +(int)I;
  int moveLeft = setting.speed - moveVal;
  int moveRight = setting.speed + moveVal;
  if (moveLeft < listPID[setting.numPID].PMin)  moveLeft = listPID[setting.numPID].PMin;
  if (moveLeft > listPID[setting.numPID].PMax)  moveLeft = listPID[setting.numPID].PMax;
  if (moveRight < listPID[setting.numPID].PMin)  moveRight = listPID[setting.numPID].PMin;
  if (moveRight > listPID[setting.numPID].PMax)  moveRight = listPID[setting.numPID].PMax;
  setMotor(moveLeft, moveRight);
  g_L = moveLeft;
  g_R = moveRight;
}

void setMotor(int16_t L, int16_t R) {
  R = -R*2.55;
  L = -L*2.55; // wheel dir adjustment
  if (L > 0) {
    digitalWrite(dirL, 0);
  } else {
    L = L + 255;
    digitalWrite(dirL, 1);
  }
  analogWrite(pwmL, L);

  if (R > 0) {
    digitalWrite(dirR, 0);
  } else {
    R = R + 255;
    digitalWrite(dirR, 1);
  }
  analogWrite(pwmR, R);
}

 
void stepResponseControl() {
  error = errorA(sensorBits);
  lastError = error;

  int moveLeft =g_L;
  int moveRight =g_R;


  if (abs(error) > g_Emax) {
    moveLeft = g_B - g_U;
    moveRight = g_B + g_U;
  }
  // if(moveLeft == moveRight){
  //   moveRight = g_R +g_U;
  // }

  if (moveLeft < g_Mn) moveLeft = g_Mn;
  if (moveLeft > g_Mx) moveLeft = g_Mx;
  if (moveRight < g_Mn) moveRight = g_Mn;
  if (moveRight > g_Mx) moveRight = g_Mx;

  setMotor(moveLeft, moveRight);
  g_L = moveLeft;
  g_R = moveRight;
}

void robotControlLoop() {
  switch (g_M) {
    case 1: {
      setMotor(0, 0);
      g_L = 0;
      g_R = 0;
    } break;

    case 2: {
      setMotor(g_L, g_R);
    } break;

    case 3: {
      PID();
    } break;

    case 4: {
      stepResponseControl();
    } break;
    case 5: {
      calibrationModeLoop();
    } break;

    default: {
      setMotor(0, 0);
      g_L = 0;
      g_R = 0;
    } break;
  }
}
