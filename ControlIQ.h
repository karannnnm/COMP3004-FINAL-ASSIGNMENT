// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef CONTROL_IQ_H
#define CONTROL_IQ_H


#include <vector>
#include <QObject>

class Profile;
class BolusCalculator;


enum class BolusDeliveryStatus{
    NOT_STARTED,
    RUNNING,
    PAUSED,
    SUSPENDED,
    COMPLETED
};

class ControlIQ : public QObject {
    Q_OBJECT
public:

    friend double returnCurrentBloodGlucoseLevelToControlIQ(const ControlIQ& controlIQ);


    ControlIQ();

    void mimicGlucoseSpike();

    // updates control iq with bolus data
    void fetchBolusData(const BolusCalculator& bolusCalculator);
    void linkCurrentBloodGlucoseLevel(BolusCalculator& bolusCalculator);
    void fetchCurrentProfile(const BolusCalculator& bolusCalculator);

    double getCurrentBloodGlucose();
    double getDurationOfExtendedBolus();
    BolusDeliveryStatus getBolusStatus();

    //bolus related functions
    void startBolus();
    void pauseBolus();
    void resumeBolus();
    void suspendBolus();

    void deliverExtendedBolus();
    void deliverBasal(double insulinAmount);
    void predictBolusRequired();
    void deliverImmediateCorrectionBolus(double insulinAmt);
    void monitorGlucoseLevel(); // monitor the bglevel and based on it deliver the basal insulin

    Profile* currentProfile = nullptr;

    // Method to display the received bolus data (for debugging)
    void displayBolusData() const;
    void displayProfileData() const;

    double getIOB();
    double getInsulinFillGauge();
    void setInsulinFillGauge(double);
    void simulateIOBFluctuation();
    void simulateInsulinFillGaugeFluctuation();
    bool isGlucoseLevelSafe () const;
    double generateRandomDouble(double min = 30.0, double max = 60.0);
    bool userPaused = false;  // indicates a user-initiated pause
    
    double* currentBloodGlucoseLevel;


private:
    double IOB;
    double insulinFillGauge = 250.0;
    
    //bolus data coming from BolusCalculator
    double immediateDose = 0.0;
    double extendedDose = 0.0;
    double extendedDosePerHour = 0.0;
    double durationOfExtendedBolus = 0; //in hours

    BolusDeliveryStatus bolusStatus = BolusDeliveryStatus::NOT_STARTED;

    double bolusDelivered = 0.0;
    double totalBolusToDeliver = 0.0;
    double extendedBolus = 0.0;
    double extendedDurationSeconds = 0.0;

signals:
    void immediateDoseDelivered();
    void extendedDoseCompleted();
    void automaticBolusAdministered();

};

#endif // CONTROL_IQ_H
