#include "Sensor.h"
#include <iostream>
#include <algorithm>

// std::clamp is C++17, provide our own for C++14
namespace {
    template<class T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }
}

namespace LidPong {

const double LidSensor::MIN_ANGLE = 0.0;   // Lid closed
const double LidSensor::MAX_ANGLE = 180.0; // Lid fully open

LidSensor::LidSensor() 
    : m_currentAngle(0.0)
    , m_sliderPosition(0.5) // Start in middle
    , m_available(false) {
    
    try {
        m_sensor = std::make_unique<MacBookLidAngle::LidAngleSensor>();
        if (m_sensor->isAvailable()) {
            m_available = true;
            std::cout << "✓ Lid angle sensor initialized successfully" << std::endl;
        } else {
            std::cout << "✗ Lid angle sensor not available" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "✗ Failed to initialize lid angle sensor: " << e.what() << std::endl;
        m_available = false;
    }
}

LidSensor::~LidSensor() {
    // Smart pointer will handle cleanup
}

bool LidSensor::isAvailable() const {
    return m_available;
}

double LidSensor::getCurrentAngle() const {
    return m_currentAngle;
}

double LidSensor::getSliderPosition() const {
    return m_sliderPosition;
}

void LidSensor::update() {
    if (!m_available || !m_sensor) {
        return;
    }
    
    try {
        m_currentAngle = m_sensor->readAngle();
        
        // Convert angle to slider position (0.0 = bottom, 1.0 = top)
        // Clamp angle to our range
        double clampedAngle = clamp(m_currentAngle, MIN_ANGLE, MAX_ANGLE);
        
        // Map angle to slider position
        m_sliderPosition = (clampedAngle - MIN_ANGLE) / (MAX_ANGLE - MIN_ANGLE);
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Failed to read lid angle: " << e.what() << std::endl;
    }
}

} // namespace LidPong
