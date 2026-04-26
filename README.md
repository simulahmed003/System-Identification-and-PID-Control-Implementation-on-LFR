# System Identification and PID Control Implementation on Line-Following Robot

> **A Data-Driven Approach to Transfer Function Identification and Simulation-Based PID Tuning for Low-Cost Differential-Drive Robots**

![Project Status](https://img.shields.io/badge/status-complete-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-Arduino%20Nano-lightgrey)

---

## 📋 Project Overview

This project presents a **systematic methodology** for system identification and PID controller design of a differential-drive line-following robot. Rather than relying on manual tuning, we combine experimental data collection, discrete-time transfer function identification, and simulation-based controller design to create a practical and reproducible workflow.

### Key Contribution
We demonstrate that real experimental telemetry data from a low-cost robot can be used to identify a compact linear plant model, which is then suitable for effective PID tuning in MATLAB/Simulink before hardware deployment.

**Project Type:** B.Sc. Engineering Project   
**Department:** Energy Science and Engineering, KUET  
**Author:** Simul Ahmed (Roll No. 2113006)  
**Supervisor:** Mostafizur Rahaman  
**Completion Date:** April 2026

---

## 🎯 Objectives

- ✅ Design, construct, and calibrate a differential-drive line-following robot with a 14-element infrared sensor array
- ✅ Develop a normalized tracking error signal for feedback control
- ✅ Identify a discrete-time transfer-function model from experimental data
- ✅ Tune PID controller gains using MATLAB and the identified model
- ✅ Validate the controller performance on the physical robot

---

## 🔧 Hardware Architecture

### Core Components
| Component | Specification | Purpose |
|-----------|---------------|---------|
| **MCU** | Arduino Nano | Central controller & signal processing |
| **Sensors** | 14x TCRT5000 IR Array | Line detection & position estimation |
| **Motor Driver** | L298N Dual H-Bridge | DC motor control |
| **Motors** | 2x N20 Geared DC Motors | Differential-drive locomotion |
| **Power** | Li-Po Battery (12V→5V) | System power supply |
| **Communication** | HC-05 Bluetooth Module | Telemetry & real-time data logging |

### System Block Diagram
```
┌─────────────────────────────────────────────────────────┐
│                  LINE FOLLOWING ROBOT                    │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  IR Sensor Array ──→ [Multiplexer] ──→ Arduino Nano      │
│                              ↓              ↓             │
│                        [Error Est.]    [PID Control]      │
│                                             ↓             │
│  L298N Motor Driver ←─────────────────────┘              │
│       ↓                    ↓                              │
│   Left Motor         Right Motor                         │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │  HC-05 Bluetooth → PC (Telemetry & Logging)        │ │
│  └─────────────────────────────────────────────────────┘ │
│                                                           │
└─────────────────────────────────────────────────────────┘
```

---

## 📊 Methodology

### 1. **Sensor Calibration**
- Each of 14 IR sensors calibrated using white/black reference surfaces
- Per-sensor thresholds computed to compensate for gain/offset variation
- Binary detection signals generated via threshold comparison

### 2. **Error Estimation**
- Weighted center-of-mass calculation for line position:
  ```
  e_raw = Σ(position × detection) / Σ(detection)
  ```
- Normalized to [-100, +100] range for control compatibility

### 3. **Control Structure**
- **Base Speed:** `u₀` (constant reference speed)
- **Steering Correction:** `uₒ(k)` (PID output)
- **Wheel Commands:**
  - Left: `L(k) = u₀ - uₒ(k)`
  - Right: `R(k) = u₀ + uₒ(k)`

### 4. **System Identification**
- **Plant Input:** Differential steering command `uₛ(k) = (L(k) - R(k))/2`
- **Plant Output:** Measured tracking error `e(k)`
- **Model Type:** Discrete-time transfer function
- **Sampling Time:** Tₛ = 0.05 s

### 5. **Identified Plant Model**

```
        0.046412·z⁻¹ + 0.204064·z⁻²
G(z) = ────────────────────────────────
       1 - 0.794225·z⁻¹ + 0.466904·z⁻²

Fit: 56.1% | RMSE: 9.77
```

### 6. **PID Tuning (MATLAB)**
- **Kₚ** (Proportional): 1.1639
- **Kᵢ** (Integral): 1.48
- **Kₐ** (Derivative): -1.2571
- **Filter Coefficient (N)**: 0.64316

---

## 📁 Repository Structure

```
System-Identification-and-PID-Control-Implementation-on-LFR/
│
├── 📄 README.md                          # Project overview (this file)
├── 📄 LICENSE                            # MIT License
├── 📄 CONTRIBUTING.md                    # Contribution guidelines
├── 📄 CHANGELOG.md                       # Version history
│
├── 📋 docs/
│   ├── PROJECT_REPORT.pdf                # Complete thesis (16 chapters)
│   ├── PRESENTATION.pdf                  # Defense presentation slides
│   ├── QUICK_START.md                    # Quick start guide
│   ├── HARDWARE_SETUP.md                 # Hardware assembly & calibration
│   ├── DATA_COLLECTION.md                # Telemetry logging procedures
│   ├── THEORY.md                         # Mathematical background
│   ├── TROUBLESHOOTING.md                # Common issues & fixes
│   └── REFERENCES.md                     # Bibliography & citations
│
├── 🖼️ Figures/
│   ├── Hardware/
│   │   ├── hardware_block_diagram.png
│   │   ├── sensor_layout.png
│   │   ├── control_structure.png
│   │   └── robot_photo.jpg
│   ├── Results/
│   │   ├── sensor_calibration_results.png
│   │   ├── system_identification_data.png
│   │   ├── tf_prediction_vs_actual.png
│   │   ├── pid_validation_plot.png
│   │   └── frequency_response.png
│   └── Theory/
│       ├── pid_structure_diagram.png
│       ├── identification_workflow.png
│       └── pid_tuning_workflow.png
│
├── 💻 firmware/
│   ├── README.md                         # Firmware documentation
│   ├── line_follower_main.ino            # Main Arduino sketch
│   ├── sensor_calibration.ino            # Calibration routine
│   ├── pid_controller.h                  # PID implementation header
│   ├── motor_controller.h                # Motor control header
│   ├── bluetooth_telemetry.h             # Bluetooth comm header
│   ├── config.h                          # Pin & parameter configuration
│   └── libraries/                        # Required Arduino libraries
│
├── 📊 data/
│   ├── README.md                         # Data documentation
│   ├── raw/
│   │   ├── experiment_001_raw.csv        # Raw sensor + command data
│   │   ├── experiment_002_raw.csv
│   │   └── ...
│   ├── processed/
│   │   ├── identification_dataset.csv    # Cleaned for identification
│   │   └── validation_dataset.csv        # Validation data
│   └── metadata.json                     # Data collection parameters
│
├── 🔬 analysis/
│   ├── README.md                         # Analysis scripts documentation
│   ├── matlab/
│   │   ├── system_identification.m       # SI using MATLAB System ID Toolbox
│   │   ├── pid_tuning.m                  # PID Tuner integration script
│   │   ├── validation_analysis.m         # Closed-loop performance analysis
│   │   └── visualization.m               # Result plotting
│   ├── python/
│   │   ├── data_processing.py            # Clean & process raw data
│   │   ├── transfer_function.py          # TF analysis utilities
│   │   ├── bode_plot.py                  # Frequency response plotting
│   │   └── requirements.txt              # Python dependencies
│   └── results/
│       ├── transfer_function_fit.txt
│       ├── pid_gains.txt
│       └── performance_metrics.txt
│
├── 📚 docs_extended/
│   ├── SYSTEM_IDENTIFICATION_THEORY.md   # Detailed SI methodology
│   ├── PID_CONTROL_THEORY.md             # PID fundamentals & tuning
│   ├── DISCRETE_TIME_SYSTEMS.md          # Z-domain analysis
│   └── KALMAN_FILTER_NOTES.md            # (Future improvement)
│
├── 🎥 media/
│   ├── robot_assembly.mp4                # Assembly guide video
│   ├── calibration_demo.mp4              # Sensor calibration
│   ├── line_following_demo.mp4           # Robot in action
│   └── screenshots/                      # MATLAB/Simulink captures
│
├── ✅ tests/
│   ├── README.md                         # Testing documentation
│   ├── unit_tests/
│   │   ├── test_pid_controller.cpp       # C++ unit tests
│   │   └── test_sensor_reading.cpp
│   └── integration_tests/
│       └── test_closed_loop.cpp
│
└── 🛠️ tools/
    ├── data_logger.py                    # PC-side data logging utility
    ├── real_time_plotter.py              # Live telemetry visualization
    ├── model_validator.m                 # MATLAB validation script
    └── calibration_utility.py            # Sensor calibration helper
```

---

## 🚀 Quick Start

### 1. **Hardware Assembly**
- See [HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md) for detailed instructions
- Expected time: 4-6 hours

### 2. **Upload Firmware**
```bash
# Clone the repository
git clone https://github.com/simulahmed003/System-Identification-and-PID-Control-Implementation-on-LFR.git
cd System-Identification-and-PID-Control-Implementation-on-LFR

# Open Arduino IDE
# Load firmware/line_follower_main.ino
# Select: Board: Arduino Nano | Processor: ATmega328P | Programmer: AVRISP mkII
# Click Upload
```

### 3. **Calibrate Sensors**
1. Upload `firmware/sensor_calibration.ino`
2. Follow on-screen instructions via Serial Monitor
3. Place robot on white/black surfaces
4. Record calibration values

### 4. **Collect System ID Data**
- See [DATA_COLLECTION.md](docs/DATA_COLLECTION.md)
- Run robot on track with base speed fixed
- Record telemetry via Bluetooth
- Save as CSV for analysis

### 5. **Identify Transfer Function**
```matlab
% In MATLAB
cd analysis/matlab/
system_identification
% Follow script prompts to load data and identify model
```

### 6. **Tune PID Gains**
```matlab
pid_tuning
% Import identified model and use MATLAB PID Tuner
```

### 7. **Deploy & Validate**
- Update PID gains in `firmware/config.h`
- Re-upload firmware
- Test on track and record performance

---

## 📈 Results Summary

### Identification Performance
- **Model Order:** 2nd-order discrete-time
- **Fit Quality:** 56.1%
- **RMSE:** 9.77
- **Sampling Time:** 50 ms

### Closed-Loop Performance (Hardware Test)
- **Mean Absolute Error:** ~34
- **RMSE:** ~40.9
- **Error Range:** [-75, +75]
- **Track Success:** ✓ Maintained line following

### Simulation vs Hardware
| Metric | Simulation | Hardware |
|--------|-----------|----------|
| Rise Time | 3.8 s | ~5.2 s |
| Settling Time | 8.8 s | ~11 s |
| Overshoot | 0% | <5% |
| Stability | ✓ Stable | ✓ Stable |

---

## 🔍 Key Findings

1. **Linear Model Adequacy:** A 2nd-order discrete transfer function captured dominant robot dynamics with 56% fit
2. **Identification-Based Tuning:** More systematic than manual tuning; produces working controller on first deployment
3. **Hardware-Model Gap:** Motor dead-zone, saturation, and sensor quantization caused 30% performance degradation vs. simulation
4. **Practical Value:** Despite gaps, the approach provides a reproducible, documented workflow

---

## 📖 Documentation

- **[PROJECT_REPORT.pdf](docs/PROJECT_REPORT.pdf)** - Complete 16-chapter thesis
- **[HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md)** - Assembly & calibration guide
- **[THEORY.md](docs/THEORY.md)** - Mathematical foundations
- **[DATA_COLLECTION.md](docs/DATA_COLLECTION.md)** - Telemetry procedures
- **[QUICK_START.md](docs/QUICK_START.md)** - 30-minute beginner guide

---

## 🛠️ Technologies & Tools

- **Microcontroller:** Arduino Nano (ATmega328P)
- **Firmware:** C/C++ (Arduino IDE)
- **System Identification:** MATLAB System Identification Toolbox
- **Control Design:** MATLAB Control System Toolbox & PID Tuner
- **Data Processing:** Python (NumPy, Pandas, SciPy)
- **Simulation:** MATLAB/Simulink
- **PCB Design:** Altium Designer (custom board)
- **Fabrication:** Cirqoid PCB milling machine

---

## 📊 Key Metrics

```
Hardware Specifications:
├── Processing: Arduino Nano @ 16 MHz
├── Sensors: 14× TCRT5000 IR (multiplexed)
├── Sampling Rate: 20 Hz (50 ms)
├── Motor Commands: 8-bit (0-255 equivalent)
├── Communication: Bluetooth 2.0 (HC-05)
└── Power Budget: ~2.5W average

Control Performance:
├── Model Fit: 56.1%
├── Closed-Loop Stability: ✓
├── Line Tracking Success: ✓
└── Response Settling Time: ~8.8 s (sim), ~11 s (hw)
```

---

## 🔄 Workflow Overview

```
┌──────────────────────────────────────────────────────────┐
│ EXPERIMENTAL DATA COLLECTION (Robot on Track)           │
│ → Telemetry logged via Bluetooth                        │
│ → CSV format: [time, L_cmd, R_cmd, e_measured]         │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ DATA PREPROCESSING & VALIDATION                         │
│ → Remove outliers                                       │
│ → Compute differential input: uₛ = (L-R)/2             │
│ → Split into ID & validation datasets                  │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ SYSTEM IDENTIFICATION (MATLAB)                          │
│ → Test multiple model structures                       │
│ → Estimate parameters via prediction error method      │
│ → Select: 2nd-order discrete TF                        │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ MODEL VALIDATION                                        │
│ → Cross-validate on independent dataset                │
│ → Assess prediction accuracy (56.1% fit)              │
│ → Residual analysis                                    │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ PID TUNING (MATLAB PID Tuner)                           │
│ → Import identified model                              │
│ → Specify performance objectives                       │
│ → Auto-tune: Kₚ=1.1639, Kᵢ=1.48, Kₐ=-1.2571          │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ SIMULATION & VALIDATION (Simulink)                     │
│ → Closed-loop simulation with tuned PID               │
│ → Step response: Rise=3.8s, Settling=8.8s, OS=0%     │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ HARDWARE DEPLOYMENT                                    │
│ → Upload updated firmware with tuned gains            │
│ → Test on physical robot                              │
│ → Log performance data                                │
└──────────────────┬───────────────────────────────────────┘
                   ↓
┌──────────────────────────────────────────────────────────┐
│ EXPERIMENTAL VALIDATION & ANALYSIS                     │
│ → Closed-loop performance: MAE~34, RMSE~40.9          │
│ → Compare sim vs hardware                              │
│ → Document findings & limitations                      │
└──────────────────────────────────────────────────────────┘
```

---

## 🐛 Known Limitations

1. **Motor Dead-Zone:** ~±15% command range (friction & driver effects)
2. **Actuator Saturation:** Limits steering aggressiveness at high speeds
3. **Sensor Quantization:** 14-element array limits position resolution
4. **Linear Model:** Cannot represent hysteresis, coulomb friction, or varying friction
5. **Environmental:** Performance degrades on tracks with poor contrast or changing lighting

---

## 🔮 Future Improvements

- [ ] Nonlinear system identification (e.g., NARX, neural networks)
- [ ] Gain scheduling for variable base speeds
- [ ] Explicit anti-windup for integral term
- [ ] Comparison with other strategies (LQR, MPC, fuzzy control)
- [ ] Kalman filtering for state estimation
- [ ] Extended Kalman Filter (EKF) implementation
- [ ] Real-time parameter adaptation
- [ ] Multi-objective optimization (speed vs accuracy)

---

## 📜 License

This project is licensed under the **MIT License** – see [LICENSE](LICENSE) file for details.

---

## 👤 Author & Contributors

**Project Lead:** Simul Ahmed  
- 🎓 B.Sc. Candidate, Energy Science & Engineering, KUET  
- 📧 [Your Email]  
- 🔗 [GitHub Profile](https://github.com/simulahmed003)

**Supervisor:** Mostafizur Rahaman  
- Lecturer, Department of Energy Science and Engineering, KUET

---

## 📞 Support & Collaboration

- **Issues & Bug Reports:** [GitHub Issues](../../issues)
- **Discussions:** [GitHub Discussions](../../discussions)
- **Email:** [Your Email]

For questions about:
- 🛠️ **Hardware Setup** → See [HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md)
- 🔬 **Experiments** → See [DATA_COLLECTION.md](docs/DATA_COLLECTION.md)
- 📊 **Data Analysis** → See `analysis/` folder
- 📚 **Theory** → See [THEORY.md](docs/THEORY.md)

---

## 📍 Citation

If you use this project in academic or professional work, please cite:

```bibtex
@thesis{ahmed2026,
  author       = {Simul Ahmed},
  title        = {System Identification and PID Control Implementation on Line-Following Robot},
  school       = {Khulna University of Engineering \& Technology},
  year         = {2026},
  department   = {Department of Energy Science and Engineering},
  url          = {https://github.com/simulahmed003/System-Identification-and-PID-Control-Implementation-on-LFR}
}
```

---

## 🙏 Acknowledgments

- Khulna University of Engineering & Technology (KUET) for facilities and support
- Supervisor Mostafizur Rahaman for guidance and feedback
- Open-source Arduino and MATLAB communities
- Cirqoid PCB milling community for fabrication insights

---

## 📊 Project Statistics

- **Total Project Duration:** 6 months
- **Lines of Code (Firmware):** ~800
- **MATLAB Scripts:** 6 main analysis files
- **Data Collected:** 47 experimental runs, ~50 MB raw data
- **Report Pages:** 16 chapters + appendices
- **Hardware Cost:** ~$85 USD

---

**Last Updated:** April 2026  
**Repository Status:** Active & Maintained  

⭐ If you found this project useful, please consider starring the repository!
