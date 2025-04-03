#ifndef USB_H
#define USB_H

#include <chrono>

class USBConnection {
public:
    explicit USBConnection();
    bool checkChargingStatus() const;
    int getBatteryLevel() const;  // Function to get the battery level for display
    void startCharging();
    void stopCharging();
    void checkBatteryNeedsCharging();
    void simulateBatteryDrain();
    void connectCharger();  // Method to simulate charger connection
    void checkAndPromptForCharger();  // Check battery and prompt for charger when below 60%

private:
    bool isCharging;
    int batteryLevel;
    void updateBatteryLevel();
    void reduceBatteryLevel(); // Simulates battery drain over time
    void simulateTimeElapsed(); // Simulates time passing for battery drain
};

#endif // USB_H

