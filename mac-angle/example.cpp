//
//  example.cpp
//  MacBook Lid Angle Sensor C++ Library Example
//
//  Example program demonstrating how to use the MacBook lid angle sensor library
//

#include "angle.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

using namespace MacBookLidAngle;

void printWelcomeMessage() {
    std::cout << "MacBook Lid Angle Sensor C++ Library Example" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Library version: " << LidAngleSensor::getVersion() << std::endl;
    std::cout << std::endl;
}

void checkDeviceCompatibility() {
    std::cout << "Checking device compatibility..." << std::endl;
    
    bool supported = LidAngleSensor::isDeviceSupported();
    if (supported) {
        std::cout << "✓ This MacBook appears to support the lid angle sensor." << std::endl;
    } else {
        std::cout << "✗ This MacBook may not support the lid angle sensor." << std::endl;
        std::cout << "  Supported devices: MacBook Pro 2019 16-inch and newer" << std::endl;
        std::cout << "  Known issues: M1 MacBook Air/Pro have compatibility problems" << std::endl;
    }
    std::cout << std::endl;
}

void demoBasicReading() {
    std::cout << "Demo: Basic angle reading" << std::endl;
    std::cout << "------------------------" << std::endl;
    
    try {
        LidAngleSensor sensor;
        
        if (!sensor.isAvailable()) {
            std::cout << "✗ Sensor is not available" << std::endl;
            return;
        }
        
        std::cout << "✓ Sensor initialized successfully" << std::endl;
        
        // Read angle once
        double angle = sensor.readAngle();
        std::cout << "Current lid angle: " << std::fixed << std::setprecision(2) 
                  << angle << "°" << std::endl;
        
    } catch (const SensorNotSupportedException& e) {
        std::cout << "✗ Sensor not supported: " << e.what() << std::endl;
    } catch (const SensorInitializationException& e) {
        std::cout << "✗ Initialization failed: " << e.what() << std::endl;
    } catch (const SensorReadException& e) {
        std::cout << "✗ Read failed: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

void demoContinuousReading() {
    std::cout << "Demo: Continuous angle monitoring" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Reading lid angle every 0.5 seconds. Press Ctrl+C to stop..." << std::endl;
    std::cout << std::endl;
    
    try {
        LidAngleSensor sensor;
        
        if (!sensor.isAvailable()) {
            std::cout << "✗ Sensor is not available for continuous monitoring" << std::endl;
            return;
        }
        
        double lastAngle = -1.0;
        
        while (true) {
            try {
                double currentAngle = sensor.readAngle();
                
                // Only print if angle changed significantly (more than 0.5 degrees)
                if (std::abs(currentAngle - lastAngle) > 0.01 || lastAngle == -1.0) {
                    auto now = std::chrono::system_clock::now();
                    auto time_t = std::chrono::system_clock::to_time_t(now);
                    auto tm = *std::localtime(&time_t);
                    
                    std::cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] "
                              << "Lid angle: " << std::fixed << std::setprecision(2)
                              << currentAngle << "°";
                    
                    if (lastAngle != -1.0) {
                        double delta = currentAngle - lastAngle;
                        std::cout << " (Δ " << std::showpos << std::setprecision(2) 
                                  << delta << "°)" << std::noshowpos;
                    }
                    
                    std::cout << std::endl;
                    lastAngle = currentAngle;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } catch (const SensorReadException& e) {
                std::cout << "Read error: " << e.what() << std::endl;
            }
        }
        
    } catch (const SensorNotSupportedException& e) {
        std::cout << "✗ Sensor not supported: " << e.what() << std::endl;
    } catch (const SensorInitializationException& e) {
        std::cout << "✗ Initialization failed: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Unexpected error: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    printWelcomeMessage();
    
    // Check command line arguments
    bool runContinuous = false;
    if (argc > 1 && std::string(argv[1]) == "--continuous") {
        runContinuous = true;
    }
    
    checkDeviceCompatibility();
    demoBasicReading();
    
    if (runContinuous) {
        demoContinuousReading();
    } else {
        std::cout << "Tip: Run with --continuous flag for continuous monitoring demo" << std::endl;
        std::cout << "     ./lid_angle_example --continuous" << std::endl;
    }
    
    return 0;
}
