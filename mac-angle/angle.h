//
//  angle.h
//  MacBook Lid Angle Sensor C++ Library
//
//  A C++ library for reading MacBook lid angle sensor data
//  Based on reverse-engineered HID device specifications
//

#pragma once

#include <exception>
#include <string>
#include <memory>

// Forward declaration for IOKit types to avoid exposing IOKit headers in public API
typedef struct __IOHIDDevice * IOHIDDeviceRef;

namespace MacBookLidAngle {

/**
 * Exception thrown when the lid angle sensor is not supported on this device
 */
class SensorNotSupportedException : public std::exception {
public:
    explicit SensorNotSupportedException(const std::string& message)
        : message_("Lid angle sensor not supported: " + message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

/**
 * Exception thrown when sensor initialization fails
 */
class SensorInitializationException : public std::exception {
public:
    explicit SensorInitializationException(const std::string& message)
        : message_("Sensor initialization failed: " + message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

/**
 * Exception thrown when sensor read operation fails
 */
class SensorReadException : public std::exception {
public:
    explicit SensorReadException(const std::string& message)
        : message_("Sensor read failed: " + message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
private:
    std::string message_;
};

/**
 * MacBook Lid Angle Sensor interface
 * 
 * This class provides access to the MacBook's internal lid angle sensor
 * through the IOKit HID framework.
 * 
 * Device Specifications:
 * - Apple device: VID=0x05AC, PID=0x8104
 * - HID Usage: Sensor page (0x0020), Orientation usage (0x008A)
 * - Data format: 16-bit angle value in centidegrees (0.01° resolution)
 * - Range: 0-360 degrees
 * 
 * Supported devices:
 * - MacBook Pro 2019 16-inch and newer
 * - MacBook Pro M2/M3/M4 series
 * 
 * Known incompatible devices:
 * - M1 MacBook Air/Pro (sensor access limitations)
 */
class LidAngleSensor {
public:
    /**
     * Constructor - automatically initializes and connects to the sensor
     * 
     * @throws SensorNotSupportedException if sensor hardware is not available
     * @throws SensorInitializationException if sensor initialization fails
     */
    LidAngleSensor();
    
    /**
     * Destructor - automatically releases resources
     */
    ~LidAngleSensor();
    
    // Disable copy constructor and assignment operator
    LidAngleSensor(const LidAngleSensor&) = delete;
    LidAngleSensor& operator=(const LidAngleSensor&) = delete;
    
    // Enable move constructor and assignment operator
    LidAngleSensor(LidAngleSensor&& other) noexcept;
    LidAngleSensor& operator=(LidAngleSensor&& other) noexcept;
    
    /**
     * Check if the sensor is available and ready to read
     * 
     * @return true if sensor is available, false otherwise
     */
    bool isAvailable() const noexcept;
    
    /**
     * Read the current lid angle
     * 
     * @return Angle in degrees (0-360)
     * @throws SensorReadException if read operation fails
     * @throws SensorNotSupportedException if sensor is not available
     */
    double readAngle();
    
    /**
     * Check if this device is expected to have a lid angle sensor
     * 
     * @return true if device model should support the sensor
     */
    static bool isDeviceSupported();
    
    /**
     * Get library version information
     * 
     * @return version string
     */
    static std::string getVersion();

private:
    class Impl; // Forward declaration for PIMPL idiom
    std::unique_ptr<Impl> pImpl;
};

} // namespace MacBookLidAngle
