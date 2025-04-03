// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>
#include <QTimer>
#include <algorithm>

// Manages charging, and simulates battery drain using QTimer
class USBConnection : public QObject {
    Q_OBJECT
public:
    // Constructor
    explicit USBConnection(QObject* parent = nullptr);

    // Returns true if the device is currently charging
    bool checkChargingStatus() const;

    // Returns the current battery level
    int getBatteryLevel() const;

    // Starts the charging process using QTimer to update battery level periodically
    void startCharging();

    // Stops the charging process
    void stopCharging();

    // Begins the battery drain simulation
    void simulateBatteryDrain();

    // Simulates the connection of a charger and initiates charging
    void connectCharger();

    // Checks if battery level is below 60% and, if it isnt charging, prompts for charger connection
    void checkAndPromptForCharger();

signals:
    void batteryFullyCharged();


private slots:
    // Called periodically by chargingTimer to increase battery level
    void updateBatteryLevel();

    // Called periodically by drainTimer to simulate battery drain
    void reduceBatteryLevel();

private:
    bool isCharging;       // True if charging is in progress
    int batteryLevel;      // Current battery level (%)

    // QTimers
    QTimer* chargingTimer;
    QTimer* drainTimer;
};

#endif // BATTERY_H
