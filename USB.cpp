
#include "USB.h"
#include <iostream>
#include <algorithm>
#include <thread> // For sleep simulation
#include <chrono> // For timing

USBConnection::USBConnection() : isCharging(false), batteryLevel(70) {}

bool USBConnection::checkChargingStatus() const {
    return isCharging;
}

int USBConnection::getBatteryLevel() const {
    return batteryLevel;
}

void USBConnection::startCharging() {
    if (!isCharging) {
        isCharging = true;
        std::cout << "Charging started.\n";
        // Simulate charging with 10% increase every second
        while (isCharging && batteryLevel < 100) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            updateBatteryLevel();  // Update battery level each second
        }
        if (batteryLevel >= 100) {
            std::cout << "Battery fully charged!\n";
        }
    }
}

void USBConnection::stopCharging() {
    if (isCharging) {
        isCharging = false;
        std::cout << "Charging stopped.\n";
    }
}

void USBConnection::updateBatteryLevel() {
    if (isCharging && batteryLevel < 100) {
        // Increase battery level by 10% each time when charging
        batteryLevel = std::min(batteryLevel + 10, 100);
        std::cout << "Battery level: " << batteryLevel << "%\n";
    }
}

void USBConnection::checkBatteryNeedsCharging() {
    if (batteryLevel <= 20) {
        std::cout << "Warning: Battery is low. Please charge soon.\n";
    }
}

void USBConnection::simulateBatteryDrain() {
    while (!isCharging && batteryLevel > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate time passing (2 seconds)
        reduceBatteryLevel();
        checkAndPromptForCharger(); // Check battery level during draining
    }
}

void USBConnection::reduceBatteryLevel() {
    if (batteryLevel > 0) {
        batteryLevel -= 1;  // Reduce battery by 1% per cycle
        std::cout << "Battery level: " << batteryLevel << "% (draining)\n";
    }
}

void USBConnection::connectCharger() {
    std::cout << "Please connect the charger.\n";
    
    // Simulate waiting for the charger to be connected
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Simulating user action of connecting charger

    // Now start charging after connecting the charger
    std::cout << "Charger connected.\n";
    startCharging();
}

// New method to check if battery is below 60% and prompt for charger
void USBConnection::checkAndPromptForCharger() {
    if (batteryLevel <= 60 && !isCharging) {
        std::cout << "Battery is below 60%. Please connect the charger.\n";
        connectCharger();
    }
}

