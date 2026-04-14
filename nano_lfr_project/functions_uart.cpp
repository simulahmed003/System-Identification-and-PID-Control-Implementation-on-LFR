#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "functions.h"

SoftwareSerial SoftSerial(softRxPin, softTxPin);

int g_L = 0;
int g_R = 0;
uint8_t g_M = 1;
uint8_t g_S = 1;

float g_P = 0.0f;
float g_I = 0.0f;
float g_D = 0.0f;

int g_B = 0;
int g_Mn = -80;
int g_Mx = 95;
int g_Sn = 0;
int g_U = 25;
int16_t g_Emax = 70;

bool g_newFrame = false;

static const size_t RX_FRAME_BUF_SIZE = 128;

struct RxState {
  char frameBuf[RX_FRAME_BUF_SIZE];
  size_t frameIndex;
  char pendingFrame[RX_FRAME_BUF_SIZE];
  bool frameReady;
  bool inFrame;
};

static RxState hardRx = {{0}, 0, {0}, false, false};
static RxState softRx = {{0}, 0, {0}, false, false};

static inline int16_t clampInt16(long v, int16_t lo, int16_t hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return (int16_t)v;
}

static inline uint8_t clampU8(long v, uint8_t lo, uint8_t hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return (uint8_t)v;
}

static void stripSpaces(char *s) {
  char *d = s;
  while (*s) {
    if (*s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') {
      *d++ = *s;
    }
    s++;
  }
  *d = '\0';
}

static void printBoth(const char *s) {
  Serial.print(s);
  SoftSerial.print(s);
}

static void printlnBoth(const char *s) {
  Serial.println(s);
  SoftSerial.println(s);
}

static void printBothInt(long v) {
  Serial.print(v);
  SoftSerial.print(v);
}

static void printBothFloat(float v, uint8_t digits) {
  Serial.print(v, digits);
  SoftSerial.print(v, digits);
}

static void printBothChar(char c) {
  Serial.print(c);
  SoftSerial.print(c);
}

static void printBinary16Both() {
  for (int i = 15; i >= 0; i--) {
    printBothInt(bitRead(sensorBits, i));
  }
}

static void applyRuntimeSettings() {
  setPID(PID_0, g_P, g_I, g_D, (byte)constrain(g_Mx, 0, 255), g_Mn);
  setting.speed = g_B;
  setting.numPID = PID_0;
  setting.sensor_sensitivity = (byte)constrain(g_Sn, 0, 100);
}

static void applyKeyValue(char key, const char *valueStr) {
  switch (key) {
    case 'L': g_L = clampInt16(strtol(valueStr, NULL, 10), -100, 100); break;
    case 'R': g_R = clampInt16(strtol(valueStr, NULL, 10), -100, 100); break;
    case 'M': g_M = clampU8(strtol(valueStr, NULL, 10), 1, 10); break;
    case 'S': g_S = clampU8(strtol(valueStr, NULL, 10), 1, 10); break;
    case 'P': g_P = (float)atof(valueStr); break;
    case 'I': g_I = (float)atof(valueStr); break;
    case 'D': g_D = (float)atof(valueStr); break;
    case 'B': g_B = clampInt16(strtol(valueStr, NULL, 10), -100, 100); break;
    case 'X': g_Mn = clampInt16(strtol(valueStr, NULL, 10), -100, 100); break;
    case 'Y': g_Mx = clampInt16(strtol(valueStr, NULL, 10), -100, 100); break;
    case 'Z': g_Sn = clampInt16(strtol(valueStr, NULL, 10), 0, 100); break;
    case 'U': g_U = clampInt16(strtol(valueStr, NULL, 10), 0, 100); break;
    case 'E': g_Emax = clampInt16(strtol(valueStr, NULL, 10), 0, 100); break;
    default: break;
  }

  applyRuntimeSettings();
}

static void parseAndUpdateFrame(char *frame) {
  stripSpaces(frame);

  char *token = strtok(frame, ",");
  while (token != NULL) {
    char *colon = strchr(token, ':');
    if (colon != NULL && colon != token) {
      *colon = '\0';
      if (token[0] != '\0' && token[1] == '\0') {
        applyKeyValue(token[0], colon + 1);
      }
    }
    token = strtok(NULL, ",");
  }

  g_newFrame = true;
}

static void ingestStream(Stream &port, RxState &state) {
  while (port.available() > 0) {
    char c = (char)port.read();

    if (!state.inFrame) {
      if (c == '<') {
        state.inFrame = true;
        state.frameIndex = 0;
      }
      continue;
    }

    if (c == '>') {
      state.frameBuf[state.frameIndex] = '\0';
      if (!state.frameReady) {
        strncpy(state.pendingFrame, state.frameBuf, RX_FRAME_BUF_SIZE - 1);
        state.pendingFrame[RX_FRAME_BUF_SIZE - 1] = '\0';
        state.frameReady = true;
      }
      state.inFrame = false;
      state.frameIndex = 0;
      continue;
    }

    if (state.frameIndex < (RX_FRAME_BUF_SIZE - 1)) {
      state.frameBuf[state.frameIndex++] = c;
    } else {
      state.inFrame = false;
      state.frameIndex = 0;
    }
  }
}

void controlVarsResetToDefault() {
  g_L = 0;
  g_R = 0;
  g_M = 1;
  g_S = 1;
  g_P = 0.0f;
  g_I = 0.0f;
  g_D = 0.0f;
  g_B = 0;
  g_Mn = -50;
  g_Mx = 100;
  g_Sn = 0;
  g_U = 40;
  g_Emax = 100;
  g_newFrame = false;

  hardRx.frameIndex = 0;
  hardRx.frameReady = false;
  hardRx.inFrame = false;
  hardRx.pendingFrame[0] = '\0';
  hardRx.frameBuf[0] = '\0';

  softRx.frameIndex = 0;
  softRx.frameReady = false;
  softRx.inFrame = false;
  softRx.pendingFrame[0] = '\0';
  softRx.frameBuf[0] = '\0';

  applyRuntimeSettings();
}

void commsInit(uint32_t baud) {
  Serial.begin(baud);
  SoftSerial.begin(baud);
  delay(200);

  controlVarsResetToDefault();

  printlnBoth("Robot UART Protocol READY");
  printlnBoth("Format: <L:100,R:-100,M:2,S:1,P:6.0,I:0.0,D:35.0,B:120,X:-120,Y:255,Z:50,U:40,E:20>");
  printlnBoth("Modes M:1 stop, 2 direct, 3 pid, 4 step-response | S:1 basic, 2 raw16, 3 bits, 4 pidcfg");
}

void commsPoll() {
  ingestStream(Serial, hardRx);
  ingestStream(SoftSerial, softRx);

  if (hardRx.frameReady) {
    parseAndUpdateFrame(hardRx.pendingFrame);
    hardRx.frameReady = false;
  }

  if (softRx.frameReady) {
    parseAndUpdateFrame(softRx.pendingFrame);
    softRx.frameReady = false;
  }
}

void commsDebugPrintIfUpdated() {
  if (!g_newFrame) return;
  g_newFrame = false;

  printBoth("Updated: L="); printBothInt(g_L);
  printBoth(" R="); printBothInt(g_R);
  printBoth(" M="); printBothInt(g_M);
  printBoth(" S="); printBothInt(g_S);
  printBoth(" P="); printBothFloat(g_P, 4);
  printBoth(" I="); printBothFloat(g_I, 4);
  printBoth(" D="); printBothFloat(g_D, 4);
  printBoth(" B="); printBothInt(g_B);
  printBoth(" X="); printBothInt(g_Mn);
  printBoth(" Y="); printBothInt(g_Mx);
  printBoth(" Z="); printBothInt(g_Sn);
  printBoth(" U="); printBothInt(g_U);
  printBoth(" E="); printBothInt(g_Emax);
  printBothChar('\n');
}

void telemetryLoop() {
  static uint32_t lastTxMs = 0;
  uint32_t now = millis();

  if ((uint32_t)(now - lastTxMs) < TELEMETRY_INTERVAL_MS) {
    return;
  }
  lastTxMs = now;

  switch (g_S) {
    case 1:
      printBothInt(now); printBothChar('\t');
      printBothInt(g_L); printBothChar('\t');
      printBothInt(g_R); printBothChar('\t');
      printBothInt(lastError); printBothChar('\n');
      break;

    case 2:
      printBothInt(now);
      for (int i = 0; i < 16; i++) {
        printBothChar('\t');
        printBothInt(reading[i]);
      }
      printBothChar('\n');
      break;

    case 3:
      printBothInt(now); printBothChar('\t');
      printBothInt(sensorBits); printBothChar('\t');
      printBinary16Both(); printBothChar('\t');
      printBothInt(errorD(sensorBits)); printBothChar('\n');
      break;

    case 4:
      printBothInt(now); printBothChar('\t');
      printBothFloat(g_P, 4); printBothChar('\t');
      printBothFloat(g_I, 4); printBothChar('\t');
      printBothFloat(g_D, 4); printBothChar('\t');
      printBothInt(g_B); printBothChar('\t');
      printBothInt(g_Mn); printBothChar('\t');
      printBothInt(g_Mx); printBothChar('\t');
      printBothInt(g_Sn); printBothChar('\t');
      printBothInt(g_U); printBothChar('\t');
      printBothInt(g_Emax); printBothChar('\n');
      break;

    default:
      break;
  }
}
