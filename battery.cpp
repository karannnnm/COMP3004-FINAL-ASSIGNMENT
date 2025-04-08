// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#include "battery.h"
#include <QDebug>
#include <QTimer>

// Constructor
USBConnection::USBConnection(QObject* parent)
    : QObject(parent), isCharging(false), batteryLevel(100)
{
    // Create timers for charging and draining
    chargingTimer = new QTimer(this);
    drainTimer = new QTimer(this);

    // Connect the charging timer's timeout signal to updateBatteryLevel()
    connect(chargingTimer, &QTimer::timeout, this, &USBConnection::updateBatteryLevel);
    // Connect the drain timer's timeout signal to reduceBatteryLevel()
    connect(drainTimer, &QTimer::timeout, this, &USBConnection::reduceBatteryLevel);

    qDebug() << "USBConnection initialized with battery level:" << batteryLevel << "% and not charging.";
}

// Returns true if the device is currently charging
bool USBConnection::checkChargingStatus() const {
    qDebug() << "Charging status checked:" << (isCharging ? "Charging" : "Not charging");
    return isCharging;
}

// Returns the current battery level
int USBConnection::getBatteryLevel() const {
    return batteryLevel;
}

// Starts the charging process if not already charging
void USBConnection::startCharging() {
    if (!isCharging) {
        isCharging = true;
        qDebug() << "Charging started. Battery level:" << batteryLevel << "%";

        // Stop the drain timer if it's active
        if (drainTimer->isActive()) {
            drainTimer->stop();
            qDebug() << "Drain simulation stopped as charging begins.";
        }

        // Start the charging timer to update the battery level every 1 second
        chargingTimer->start(1000);
    } else {
        qDebug() << "startCharging() called, but device is already charging.";
    }
}

// Stops the charging process if it is active
void USBConnection::stopCharging() {
    if (isCharging) {
        isCharging = false;
        if (chargingTimer->isActive()) {
            chargingTimer->stop();
        }
        qDebug() << "Charging stopped. Battery level:" << batteryLevel << "%";
    } else {
        qDebug() << "stopCharging() called, but device is not charging.";
    }
}

// Increases the battery level by 10% each cycle until it reaches 100%.
// Once fully charged, stops charging and restarts battery drain simulation automatically.
void USBConnection::updateBatteryLevel() {
    if (isCharging && batteryLevel < 100) {
        batteryLevel = std::min(batteryLevel + 10, 100);
        qDebug() << "Charging update: Battery level increased to:" << batteryLevel << "%";

        if (batteryLevel >= 100) {
            qDebug() << "Battery fully charged. Stopping charging process.";
            stopCharging();
            emit batteryFullyCharged();

            // Automatically restart battery drain simulation after fully charged
            QTimer::singleShot(1000, this, [this]() {
                simulateBatteryDrain();
            });
        }
    }
}



// Starts the battery drain simulation if not charging and battery is above 0%.
void USBConnection::simulateBatteryDrain() {
    if (!isCharging && batteryLevel > 0) {
        if (!drainTimer->isActive()) {
            qDebug() << "Starting battery drain simulation.";
            drainTimer->start(1000);
        }
    } else {
        qDebug() << "simulateBatteryDrain() not started: either charging or battery is empty.";
    }
}

// Reduces the battery level by 1% per cycle
void USBConnection::reduceBatteryLevel() {
    if (!isCharging && batteryLevel > 0) {
        batteryLevel -= 1;
        qDebug() << "Battery level is now at:" << batteryLevel << "%";

        // Stop the drain timer if the battery is fully drained.
        if (batteryLevel <= 0) {
            drainTimer->stop();
            qDebug() << "Battery fully drained. Stopping battery drain simulation.";
        }
    }
}

// Simulates connecting a charger by prompting the user and then initiating charging
void USBConnection::connectCharger() {
    if (batteryLevel >= 100) {
        qDebug() << "Battery is fully charged. Charging not initiated.";
        return;
    }
    QTimer::singleShot(1000, this, [this]() {
        qDebug() << "Charger connected. Starting charging cycle.";
        startCharging();
    });
}

// Checks if the battery is below 60% and not charging, then logs a message.
void USBConnection::checkAndPromptForCharger() {
    if (batteryLevel <= 60 && !isCharging) {
        qDebug() << "Battery below 60% (" << batteryLevel << "%). (Automatic prompt disabled.)";
    }
}
