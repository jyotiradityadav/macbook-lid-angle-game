# MacBook Lid Angle Sensor C++ Library

A C++ library for reading MacBook lid angle sensor data, based on reverse engineering of HID device specifications.

## Features

- 🔍 Direct access to MacBook's built-in lid angle sensor
- 📏 Real-time precise angle measurements (0-360 degree range)
- ⚡ High-performance C++ implementation with modern C++14 standard support
- 🛡️ Comprehensive exception handling mechanism
- 🔧 Clean and easy-to-use API interface
- 📦 CMake build system support

## Device Compatibility

### Supported Devices
- MacBook Pro 16-inch (2019) and newer models
- MacBook Pro M2/M3/M4 series

### Known Incompatible Devices
- M1 MacBook Air/Pro (sensor access restricted)

### Technical Specifications
- Device identification: Apple VID=0x05AC, PID=0x8104
- HID usage: Sensor page (0x0020), Orientation usage (0x008A)
- Data format: 16-bit angle value with 0.01-degree precision
- Data range: 0-360 degrees

## Quick Start

### System Requirements

- macOS 10.15 or later
- Xcode Command Line Tools
- CMake 3.15 or later

### Build and Installation

```bash
# Clone the repository
git clone <repository-url>
cd mac-angle

# Create build directory
mkdir build && cd build

# Configure build
cmake ..

# Compile
make

# Run example program
./lid_angle_example
```

### Basic Usage

```cpp
#include "angle.h"
#include <iostream>

using namespace MacBookLidAngle;

int main() {
    try {
        // Create sensor instance
        LidAngleSensor sensor;
        
        // Check if sensor is available
        if (sensor.isAvailable()) {
            // Read current angle
            double angle = sensor.readAngle();
            std::cout << "Current lid angle: " << angle << "°" << std::endl;
        }
        
    } catch (const SensorNotSupportedException& e) {
        std::cerr << "Device not supported: " << e.what() << std::endl;
    } catch (const SensorInitializationException& e) {
        std::cerr << "Initialization failed: " << e.what() << std::endl;
    } catch (const SensorReadException& e) {
        std::cerr << "Read failed: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Continuous Monitoring

```cpp
#include "angle.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace MacBookLidAngle;

int main() {
    try {
        LidAngleSensor sensor;
        
        while (true) {
            double angle = sensor.readAngle();
            std::cout << "Angle: " << angle << "°" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

## API Documentation

### Class: `MacBookLidAngle::LidAngleSensor`

The main sensor interface class providing access to MacBook lid angle sensor.

#### Constructor

```cpp
LidAngleSensor();
```

Automatically initializes and connects to the sensor. Throws appropriate exceptions if the sensor is unavailable or initialization fails.

**Exceptions:**
- `SensorNotSupportedException` - Device doesn't support sensor hardware
- `SensorInitializationException` - Sensor initialization failed

#### Member Functions

##### `bool isAvailable() const noexcept`

Checks if the sensor is available and ready for reading.

**Returns:** `true` if sensor is available, `false` otherwise

##### `double readAngle()`

Reads the current lid angle.

**Returns:** Angle value (0-360 degrees)

**Exceptions:**
- `SensorReadException` - Read operation failed
- `SensorNotSupportedException` - Sensor unavailable

#### Static Functions

##### `static bool isDeviceSupported()`

Checks if this device should support the lid angle sensor.

**Returns:** `true` if device model should support the sensor

##### `static std::string getVersion()`

Gets library version information.

**Returns:** Version string

### Exception Classes

#### `SensorNotSupportedException`

Thrown when the lid angle sensor is not supported on this device.

#### `SensorInitializationException`

Thrown when sensor initialization fails.

#### `SensorReadException`

Thrown when sensor read operation fails.

## CMake Integration

### Using as Subdirectory

```cmake
add_subdirectory(macbook-lid-angle)
target_link_libraries(your_target macbook_lid_angle)
```

### Using After Installation

```cmake
find_package(MacBookLidAngle REQUIRED)
target_link_libraries(your_target MacBookLidAngle::macbook_lid_angle)
```

## Example Program

The project includes a complete example program `example.cpp` demonstrating:

- Device compatibility checking
- Basic angle reading
- Continuous monitoring mode

Run the example:
```bash
# Basic demonstration
./lid_angle_example

# Continuous monitoring demonstration
./lid_angle_example --continuous
```

## Troubleshooting

### "Sensor not supported" Error

1. Confirm your device is in the supported list
2. Check if other applications are using the sensor
3. Try restarting your MacBook

### "Sensor initialization failed" Error

1. Ensure your application has appropriate system permissions
2. Check macOS version compatibility
3. Try running with administrator privileges

### "Sensor read failed" Error

1. Sensor may be temporarily unavailable
2. Implement retry logic
3. Check device connection status

## Technical Implementation Details

This library uses macOS IOKit framework to directly access HID devices:

1. **Device Discovery**: Match devices by specific VID/PID and HID usage pages
2. **Device Validation**: Test device response to ensure proper functionality
3. **Data Reading**: Use Feature Reports to read 16-bit angle data
4. **Error Handling**: Comprehensive exception handling system

## Acknowledgments

This C++ library is inspired by and based on the original [**Lid Angle Sensor**](https://github.com/samhenrigold/LidAngleSensor) project by **Sam Gold**. We extend our sincere gratitude to Sam for his pioneering work in reverse engineering the MacBook lid angle sensor HID interface and making this functionality accessible to developers.

The original Objective-C implementation provided the foundation and insights that made this C++ port possible. This project aims to bring the same functionality to C++ developers while maintaining the reliability and accuracy of the original implementation.

## License

This project is developed based on the original LidAngleSensor project and follows open source licensing agreements.

## Contributing

Issues and Pull Requests are welcome!

## Related Projects

- [**Lid Angle Sensor (Objective-C)**](https://github.com/samhenrigold/LidAngleSensor) by Sam Gold - The original inspiration and foundation for this library
- [pybooklid (Python)](https://github.com/tcsenpai/pybooklid) - Python implementation

## Author

C++ port and extensions based on Sam Gold's original work.