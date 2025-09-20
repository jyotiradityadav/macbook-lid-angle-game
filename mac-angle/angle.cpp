//
//  angle.cpp
//  MacBook Lid Angle Sensor C++ Library
//
//  Implementation of the MacBook lid angle sensor interface
//

#include "angle.h"
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/IOReturn.h>
#include <CoreFoundation/CoreFoundation.h>
#include <iostream>
#include <sstream>

namespace MacBookLidAngle {

// PIMPL implementation class
class LidAngleSensor::Impl {
public:
    Impl();
    ~Impl();
    
    bool isAvailable() const noexcept;
    double readAngle();
    
private:
    IOHIDDeviceRef findLidAngleSensor();
    bool testDevice(IOHIDDeviceRef device);
    
    IOHIDDeviceRef hidDevice;
    bool deviceInitialized;
};

LidAngleSensor::Impl::Impl() : hidDevice(nullptr), deviceInitialized(false) {
    hidDevice = findLidAngleSensor();
    if (hidDevice) {
        IOReturn result = IOHIDDeviceOpen(hidDevice, kIOHIDOptionsTypeNone);
        if (result == kIOReturnSuccess) {
            deviceInitialized = true;
            std::cout << "[MacBookLidAngle] Successfully initialized lid angle sensor" << std::endl;
        } else {
            CFRelease(hidDevice);
            hidDevice = nullptr;
            throw SensorInitializationException("Failed to open HID device (IOReturn: " + std::to_string(result) + ")");
        }
    } else {
        throw SensorNotSupportedException("Lid angle sensor device not found on this MacBook");
    }
}

LidAngleSensor::Impl::~Impl() {
    if (hidDevice) {
        if (deviceInitialized) {
            IOHIDDeviceClose(hidDevice, kIOHIDOptionsTypeNone);
        }
        CFRelease(hidDevice);
        hidDevice = nullptr;
        deviceInitialized = false;
    }
}

bool LidAngleSensor::Impl::isAvailable() const noexcept {
    return hidDevice != nullptr && deviceInitialized;
}

IOHIDDeviceRef LidAngleSensor::Impl::findLidAngleSensor() {
    IOHIDManagerRef manager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (!manager) {
        throw SensorInitializationException("Failed to create IOHIDManager");
    }
    
    if (IOHIDManagerOpen(manager, kIOHIDOptionsTypeNone) != kIOReturnSuccess) {
        CFRelease(manager);
        throw SensorInitializationException("Failed to open IOHIDManager");
    }
    
    // Create matching dictionary for the lid angle sensor
    // Target: Apple VID=0x05AC, PID=0x8104, Sensor page (0x0020), Orientation usage (0x008A)
    CFMutableDictionaryRef matchingDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                   &kCFTypeDictionaryKeyCallBacks,
                                                                   &kCFTypeDictionaryValueCallBacks);
    
    // Set vendor ID (Apple)
    int vendorIDValue = 0x05AC;
    CFNumberRef vendorID = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &vendorIDValue);
    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDVendorIDKey), vendorID);
    CFRelease(vendorID);
    
    // Set product ID
    int productIDValue = 0x8104;
    CFNumberRef productID = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &productIDValue);
    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDProductIDKey), productID);
    CFRelease(productID);
    
    // Set usage page (Sensor)
    int usagePageValue = 0x0020;
    CFNumberRef usagePage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePageValue);
    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDPrimaryUsagePageKey), usagePage);
    CFRelease(usagePage);
    
    // Set usage (Orientation)
    int usageValue = 0x008A;
    CFNumberRef usage = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usageValue);
    CFDictionarySetValue(matchingDict, CFSTR(kIOHIDPrimaryUsageKey), usage);
    CFRelease(usage);
    
    IOHIDManagerSetDeviceMatching(manager, matchingDict);
    CFRelease(matchingDict);
    
    CFSetRef devices = IOHIDManagerCopyDevices(manager);
    IOHIDDeviceRef foundDevice = nullptr;
    
    if (devices && CFSetGetCount(devices) > 0) {
        std::cout << "[MacBookLidAngle] Found " << CFSetGetCount(devices) << " matching device(s)" << std::endl;
        
        CFIndex deviceCount = CFSetGetCount(devices);
        const void** deviceArray = new const void*[deviceCount];
        CFSetGetValues(devices, deviceArray);
        
        // Test each matching device to find one that works
        for (CFIndex i = 0; i < deviceCount; i++) {
            IOHIDDeviceRef candidateDevice = (IOHIDDeviceRef)deviceArray[i];
            if (testDevice(candidateDevice)) {
                foundDevice = (IOHIDDeviceRef)CFRetain(candidateDevice);
                std::cout << "[MacBookLidAngle] Successfully found working device (index " << i << ")" << std::endl;
                break;
            }
        }
        
        delete[] deviceArray;
    }
    
    if (devices) {
        CFRelease(devices);
    }
    
    IOHIDManagerClose(manager, kIOHIDOptionsTypeNone);
    CFRelease(manager);
    
    return foundDevice;
}

bool LidAngleSensor::Impl::testDevice(IOHIDDeviceRef device) {
    if (IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone) != kIOReturnSuccess) {
        return false;
    }
    
    uint8_t testReport[8] = {0};
    CFIndex reportLength = sizeof(testReport);
    
    IOReturn result = IOHIDDeviceGetReport(device,
                                          kIOHIDReportTypeFeature,
                                          1,
                                          testReport,
                                          &reportLength);
    
    IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
    
    return (result == kIOReturnSuccess && reportLength >= 3);
}

double LidAngleSensor::Impl::readAngle() {
    if (!isAvailable()) {
        throw SensorNotSupportedException("Sensor device is not available");
    }
    
    uint8_t report[8] = {0};
    CFIndex reportLength = sizeof(report);
    
    IOReturn result = IOHIDDeviceGetReport(hidDevice,
                                          kIOHIDReportTypeFeature,
                                          1,
                                          report,
                                          &reportLength);
    
    if (result != kIOReturnSuccess) {
        throw SensorReadException("Failed to read from HID device (IOReturn: " + std::to_string(result) + ")");
    }
    
    if (reportLength < 3) {
        throw SensorReadException("Invalid report length: " + std::to_string(reportLength) + " (expected >= 3)");
    }
    
    // Parse the 16-bit angle value from bytes 1-2 (skipping report ID at byte 0)
    uint16_t rawValue = (report[2] << 8) | report[1]; // High byte, low byte
    double angle = static_cast<double>(rawValue);
    
    return angle;
}

// Public interface implementation

LidAngleSensor::LidAngleSensor() : pImpl(std::make_unique<Impl>()) {
}

LidAngleSensor::~LidAngleSensor() = default;

LidAngleSensor::LidAngleSensor(LidAngleSensor&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

LidAngleSensor& LidAngleSensor::operator=(LidAngleSensor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool LidAngleSensor::isAvailable() const noexcept {
    return pImpl && pImpl->isAvailable();
}

double LidAngleSensor::readAngle() {
    if (!pImpl) {
        throw SensorNotSupportedException("Sensor object not properly initialized");
    }
    return pImpl->readAngle();
}

bool LidAngleSensor::isDeviceSupported() {
    // This is a simple heuristic - a more sophisticated implementation
    // could check the actual hardware model
    try {
        LidAngleSensor testSensor;
        return testSensor.isAvailable();
    } catch (const std::exception&) {
        return false;
    }
}

std::string LidAngleSensor::getVersion() {
    return "1.0.0";
}

} // namespace MacBookLidAngle
