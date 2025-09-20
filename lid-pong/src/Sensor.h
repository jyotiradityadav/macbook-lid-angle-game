#pragma once

#include "../mac-angle/angle.h"
#include <memory>

namespace LidPong {

class LidSensor {
public:
    LidSensor();
    ~LidSensor();
    
    bool isAvailable() const;
    double getCurrentAngle() const;
    double getSliderPosition() const; // Convert angle to slider position (0.0 to 1.0)
    
    void update();
    
private:
    std::unique_ptr<MacBookLidAngle::LidAngleSensor> m_sensor;
    double m_currentAngle;
    double m_sliderPosition;
    bool m_available;
    
    // Angle to slider position mapping
    static const double MIN_ANGLE; // Minimum angle for slider at bottom
    static const double MAX_ANGLE; // Maximum angle for slider at top
};

} // namespace LidPong
