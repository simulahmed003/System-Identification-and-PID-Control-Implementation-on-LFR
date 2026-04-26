# Firmware Documentation

This directory contains the Arduino firmware for the line-following robot control system.

---

## 📁 Files Overview

| File | Purpose |
|------|---------|
| `line_follower_main.ino` | Main control loop with PID implementation |
| `sensor_calibration.ino` | Sensor calibration utility |
| `pid_controller.h` | PID controller header (library) |
| `motor_controller.h` | Motor control header (PWM, direction) |
| `bluetooth_telemetry.h` | Bluetooth communication module |
| `config.h` | Central configuration file (pins, gains, parameters) |

---

## 🚀 Quick Upload

```bash
# 1. Open Arduino IDE
# 2. Load: firmware/line_follower_main.ino
# 3. Board: Arduino Nano | Processor: ATmega328P
# 4. Port: /dev/ttyUSB0 (or your COM port)
# 5. Click Upload
```

---

## ⚙️ Configuration

Edit `config.h` to adjust:

```cpp
// Pin Definitions
#define MOTOR_L_PWM      3
#define MOTOR_R_PWM      6
// ... (see config.h for all pins)

// PID Gains (from MATLAB tuning)
#define KP 1.1639
#define KI 1.48
#define KD -1.2571

// Base Speed
#define BASE_SPEED 60    // Range: 30-100

// Sampling Time
#define SAMPLING_TIME_MS 50  // milliseconds
```

---

## 📊 Main Sketch (`line_follower_main.ino`)

Performs:
1. Read 14 IR sensors via multiplexer
2. Calculate line position error (center-of-mass)
3. Compute PID steering command
4. Send commands to motors
5. Transmit telemetry via Bluetooth

**Loop Time:** ~50 ms average

---

## 🔧 Modules

### PID Controller (`pid_controller.h`)
```cpp
class PIDController {
    void setGains(float kp, float ki, float kd);
    int calculate(int error);
    void reset();
};
```

### Motor Control (`motor_controller.h`)
```cpp
void setMotorSpeed(int left_cmd, int right_cmd);
void motorStop();
```

### Bluetooth (`bluetooth_telemetry.h`)
```cpp
void initBluetooth();
void sendTelemetry(int L, int R, int error);
```

---

## 📝 Sample Code

```cpp
#include "config.h"
#include "pid_controller.h"
#include "motor_controller.h"

PIDController pid;

void setup() {
    Serial.begin(9600);
    initMotors();
    pid.setGains(KP, KI, KD);
}

void loop() {
    int error = readSensors();
    int steering = pid.calculate(error);
    
    int left_cmd = BASE_SPEED - steering;
    int right_cmd = BASE_SPEED + steering;
    
    setMotorSpeed(left_cmd, right_cmd);
    sendTelemetry(left_cmd, right_cmd, error);
    
    delay(SAMPLING_TIME_MS);
}
```

---

## 🔍 Debugging

Enable Serial debug output in `config.h`:
```cpp
#define DEBUG_MODE 1
```

Check Serial Monitor (9600 baud) for:
```
[INFO] Sensor 0: 512
[INFO] Error: 5
[INFO] Steering: 8
```

---

## 📦 Dependencies

No external libraries required for basic functionality.

Optional (if using advanced features):
- `PID` library (alternative implementation)

---

## 🧪 Testing

1. **Motor Test:** Upload `test_motors.ino` to verify motor connections
2. **Sensor Test:** Upload `test_sensors.ino` to check IR readings
3. **Communication:** Use Serial Plotter to visualize real-time data

---

## ⚠️ Important Notes

- **Bluetooth Pin:** Use voltage divider on RX (5V → 3.3V)
- **Motor Dead-Zone:** Set MIN_SPEED to ~20-30 to overcome friction
- **Sampling:** Loop time must be consistent (use millis() for precise timing)
- **Safety:** Always test motors before running on the robot

---

See [QUICK_START.md](../docs/QUICK_START.md) for detailed setup instructions.
