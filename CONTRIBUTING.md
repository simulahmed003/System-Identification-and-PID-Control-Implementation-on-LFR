# Contributing to System Identification & PID Control Line-Following Robot

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

---

## 📜 Code of Conduct

- **Be Respectful:** Treat all contributors with respect
- **Be Inclusive:** Welcome contributions from all backgrounds
- **Be Constructive:** Provide helpful feedback and suggestions
- **Report Issues:** Use GitHub Issues for bugs or concerns

---

## 🤝 Types of Contributions

### 1. 🐛 Bug Reports

Found a bug? Please report it!

**Before Submitting:**
- Check if the bug has already been reported
- Provide a minimal example to reproduce

**Include:**
```markdown
## Bug Report

**Title:** Brief description

**Description:** 
What happened and what did you expect?

**Steps to Reproduce:**
1. ...
2. ...

**Environment:**
- Board: Arduino Nano / Hardware setup
- Firmware version: v1.0.0
- OS: Windows/Linux/Mac

**Attachments:**
- Screenshots or error logs
```

### 2. ✨ Feature Requests

Have an idea for improvement?

**Include:**
```markdown
## Feature Request

**Title:** Concise feature name

**Description:** 
Why is this needed? How would it work?

**Example Use Case:**
How would a user benefit?

**Alternatives Considered:**
Are there other approaches?
```

### 3. 📚 Documentation

Improve docs, add examples, or fix typos.

**Areas that need help:**
- Hardware assembly guides
- MATLAB/Python tutorials
- Troubleshooting sections
- Theory explanations
- API documentation

### 4. 🔧 Code Improvements

Fix issues, optimize, or refactor code.

**Focus Areas:**
- Firmware efficiency improvements
- Analysis script optimization
- Better error handling
- Code clarity & comments

### 5. 🧪 Testing & Validation

- Hardware testing on new platforms
- Performance benchmarking
- Edge case validation
- Data collection

---

## 🚀 Getting Started

### 1. Fork the Repository
```bash
# Click "Fork" on GitHub
git clone https://github.com/YOUR_USERNAME/System-Identification-and-PID-Control-Implementation-on-LFR.git
cd System-Identification-and-PID-Control-Implementation-on-LFR
git remote add upstream https://github.com/simulahmed003/System-Identification-and-PID-Control-Implementation-on-LFR.git
```

### 2. Create a Branch
```bash
# Branch naming convention: type/description
git checkout -b fix/motor-deadzone-issue
# or
git checkout -b feature/add-kalman-filter
# or
git checkout -b docs/enhance-hardware-guide
```

### 3. Make Your Changes

See style guides below for your language/type.

### 4. Commit Your Changes
```bash
# Write clear, descriptive commit messages
git add .
git commit -m "Fix: Reduce motor dead-zone sensitivity

- Improved threshold detection algorithm
- Added calibration routine for dead-zone
- Tested on 5 different motor sets
- Reduces error by ~15%"
```

### 5. Push & Create Pull Request
```bash
git push origin fix/motor-deadzone-issue
```

Then create a PR with:
- **Title:** Clear, concise description
- **Description:** What changed, why, and how to test
- **References:** Link related issues (#42)
- **Screenshots/Videos:** If applicable

---

## 💻 Code Style Guides

### C++ / Arduino Firmware

**Naming Conventions:**
```cpp
// Constants: UPPER_CASE_WITH_UNDERSCORES
#define MAX_MOTOR_COMMAND 100
#define SAMPLING_TIME_MS 50

// Variables: camelCase
int motorLeftCommand = 60;
float proportionalGain = 1.164;

// Functions: camelCase
void setMotorSpeed(int left, int right);
int calculateSteeringError();

// Classes: PascalCase
class PIDController {
    void setGains(float kp, float ki, float kd);
};
```

**Code Format:**
```cpp
// Use 2-space indentation
void setup() {
  Serial.begin(9600);
  initMotors();
}

// One statement per line
int error = readSensors();
int steering = pid.calculate(error);
updateMotors(steering);

// Comments for complex logic
// Calculate weighted center-of-mass of sensor array
int error_raw = 0;
int total_weight = 0;
for (int i = 0; i < NUM_SENSORS; i++) {
  if (sensorDetection[i]) {
    error_raw += sensorPosition[i] * sensorDetection[i];
    total_weight += sensorDetection[i];
  }
}
```

**Function Documentation:**
```cpp
// Brief description of what function does
// 
// Parameters:
//   left_cmd - Left motor command [-100, 100]
//   right_cmd - Right motor command [-100, 100]
//
// Returns:
//   None
//
void setMotorSpeed(int left_cmd, int right_cmd) {
  // Implementation
}
```

### MATLAB Code

**Naming:**
```matlab
% Constants and configuration
MAX_ITERATIONS = 1000;
SAMPLING_TIME = 0.05;  % seconds

% Variables: camelCase
motorCommand = [60, 60];
trackingError = 5;

% Functions: camelCase, descriptive
transfer_function = identifyPlant(data);
plotClosedLoopResponse(sys, t);

% Class names: PascalCase (if using OOP)
controller = PIDController();
```

**Code Format:**
```matlab
% Use 2-space indentation
function [output] = processData(input)
    % Process sensor data
    
    % Validate input
    if isempty(input)
        error('Input data cannot be empty');
    end
    
    % Main processing
    output = input * 2;
    
    % Return with comments explaining result
end
```

**Comments:**
```matlab
% Section separator
% ==========================================

% Calculate error using center-of-mass
error_raw = sum(position .* detection) / sum(detection);

% Normalize to [-100, 100]
error_normalized = (error_raw / 7) * 100;
```

### Python Code (PEP 8)

**Naming:**
```python
# Constants: UPPER_CASE_WITH_UNDERSCORES
MAX_MOTOR_COMMAND = 100
SAMPLING_TIME_MS = 50

# Variables: snake_case
motor_left_command = 60
proportional_gain = 1.164

# Functions: snake_case
def set_motor_speed(left, right):
    pass

def calculate_steering_error():
    pass

# Classes: PascalCase
class PIDController:
    def set_gains(self, kp, ki, kd):
        pass
```

**Code Format:**
```python
# Use 4-space indentation
def process_sensor_data(sensor_array):
    """
    Calculate line position from sensor array.
    
    Args:
        sensor_array (list): Binary detection values for each sensor
    
    Returns:
        float: Normalized position error [-100, 100]
    """
    error_raw = 0
    total_weight = 0
    
    for i, detection in enumerate(sensor_array):
        if detection:
            error_raw += SENSOR_POSITIONS[i] * detection
            total_weight += detection
    
    if total_weight == 0:
        return 0  # No sensor detected line
    
    error_normalized = (error_raw / total_weight / 7) * 100
    return error_normalized
```

**Docstrings:**
```python
def identify_transfer_function(data):
    """
    Identify discrete-time transfer function from input-output data.
    
    Uses ARX (AutoRegressive with eXogenous input) model structure
    to identify a 2nd-order plant model.
    
    Args:
        data (dict): Dictionary with keys 'input' and 'output'
                     - data['input']: steering commands (N,)
                     - data['output']: tracking errors (N,)
    
    Returns:
        dict: Identified model parameters
              - 'numerator': [b1, b2]
              - 'denominator': [a0, a1, a2]
              - 'fit': percentage fit (0-100)
              - 'rmse': root mean square error
    
    Raises:
        ValueError: If data is empty or invalid
        
    Examples:
        >>> data = {'input': u, 'output': y}
        >>> result = identify_transfer_function(data)
        >>> print(result['fit'])  # 56.1
    """
    # Implementation
    pass
```

---

## 🧪 Testing Standards

### Unit Tests (C++)
```cpp
// File: tests/unit_tests/test_pid_controller.cpp
void test_pid_proportional_action() {
    PIDController pid;
    pid.setGains(1.0, 0, 0);  // P-only
    
    int error = 10;
    int output = pid.calculate(error);
    
    assert(output == 10);  // P action: output = Kp * error
}

void test_pid_reset() {
    PIDController pid;
    pid.setGains(1, 1, 0);
    
    pid.calculate(10);
    pid.calculate(10);
    pid.reset();
    
    // After reset, integral should be zero
    int output = pid.calculate(10);
    assert(output == 10);  // Only P action
}
```

### Integration Tests
- Test robot on actual track
- Verify motor response
- Check sensor calibration
- Validate Bluetooth communication

---

## 📝 Documentation Standards

### README Sections
- Overview (1-2 sentences)
- Features/Capabilities
- Quick Start
- Detailed Instructions
- Troubleshooting
- API Reference (if applicable)

### Code Comments
```cpp
// Good: Explains WHY, not WHAT
// Multiply by 100 to convert from fractional position to percentage
error_normalized = (error_raw / 7) * 100;

// Bad: Just restates code
// Multiply error_raw by 100
error_normalized = (error_raw / 7) * 100;
```

### Commit Messages
```
[type] Brief description

Detailed explanation of changes, why they're needed,
and any important notes.

Fixes: #123
Related: #456
```

**Types:** `feat:`, `fix:`, `docs:`, `style:`, `refactor:`, `test:`, `chore:`

---

## ✅ Pull Request Checklist

Before submitting a PR:

- [ ] Code follows style guide
- [ ] Comments added for complex logic
- [ ] Tests pass (if applicable)
- [ ] Documentation updated
- [ ] No unnecessary files included
- [ ] Commit messages are clear
- [ ] Related issues linked
- [ ] Changes tested on hardware (if applicable)

---

## 🔄 Review Process

1. **Automated Checks**
   - Code style verification
   - Build tests
   - Basic validation

2. **Maintainer Review**
   - Code quality
   - Documentation
   - Hardware compatibility
   - Performance impact

3. **Approval & Merge**
   - At least one approval
   - All checks passing
   - Conflicts resolved

---

## 📚 Resources

- **Project Report:** [docs/PROJECT_REPORT.pdf](docs/PROJECT_REPORT.pdf)
- **Hardware Guide:** [docs/HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md)
- **Theory:** [docs/THEORY.md](docs/THEORY.md)
- **Data:** [data/README.md](data/README.md)
- **Analysis:** [analysis/README.md](analysis/README.md)

---

## 🎯 Current Priorities

We're looking for help with:

1. **Hardware Porting**
   - [ ] Test on STM32F411CE (Line-Follower project)
   - [ ] Adapt for different sensor arrays
   - [ ] Support additional motor types

2. **Advanced Control**
   - [ ] Nonlinear system identification
   - [ ] Kalman filter implementation
   - [ ] LQR controller design
   - [ ] Gain scheduling

3. **Documentation**
   - [ ] Video tutorials
   - [ ] More assembly photos
   - [ ] Troubleshooting database
   - [ ] FAQ section

4. **Testing**
   - [ ] Unit test framework
   - [ ] CI/CD pipeline
   - [ ] Regression testing

---

## 📞 Questions?

- **Issues/Bugs:** Use [GitHub Issues](../../issues)
- **Discussions:** Use [GitHub Discussions](../../discussions)
- **Email:** simulahmed2113006@gmail.com

---

**Thank you for contributing! Your work makes this project better. 🙏**

---

## 🎓 Attribution

Contributors will be listed in:
- [CONTRIBUTORS.md](CONTRIBUTORS.md)
- Project README
- GitHub contributors page

---

## 📜 License

All contributions are licensed under the MIT License.
By submitting a contribution, you agree that your work will be licensed under the MIT License.
