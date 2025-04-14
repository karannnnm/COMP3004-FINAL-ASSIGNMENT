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




// how these enums values are working. 

//          NOT_STARTED -> RUNNING -> PAUSED -> RUNNING -> COMPLETED
//                        \-> SUSPENDED (cannot restart)



// BG Range (mmol/L)	Action
// < 3.9	     Suspend basal delivery
// 3.9 - 6.5	 Lower basal rate slightly
// 6.5 - 10	     Normal basal delivery
// > 10	         Increase basal rate slightly



/*
    step1: fetch the correct profile and its bolus dose data from BolusCalculator.
    step2: fetch the currentBloodGlucose and decrease incrementally as the bolus is being recieved.
            - keep the duration of the bolus as well.
            - amount of currentBloodGlucose to be decreased is based on the correction factor.
            - For example: if 1 unit of insulin drops glucose by 2 mmol/L, and we deliver 5 units,
                    BG could theoretically drop by 10 mmol/L over a few hours (in ideal conditions).
    step3: start bolus.
            - start timer and give the immediate dose.

    step4: cancel bolus

    step5: pause bolus

    step6: basal delivery


*/


class ControlIQ : public QObject {
    Q_OBJECT
public:

    friend double returnCurrentBloodGlucoseLevelToControlIQ(const ControlIQ& controlIQ);


    // Method to fetch bolus data from BolusCalculator
    ControlIQ();
    // Profile* currentProfile = nullptr;

    void mimicGlucoseSpike();

    //step1.
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
    // attach this function to qtimer and call it every 30 seconds.
    void deliverExtendedBolus();
    void deliverBasal(double insulinAmount);
    

    //basal related functions
    //this function will always be running for the lifetime of the simulation.
    // will continuously monitor the bglevel and based on it deliver the basal insulin
    void monitorGlucoseLevel();
    // void deliverBasal(double insulinAmount);


    Profile* currentProfile = nullptr;

    // double getCurrentBloodGlucose();

    // Method to display the received bolus data (for debugging)
    void displayBolusData() const;
    void displayProfileData() const;

    double getIOB();
    double getInsulinFillGauge();
    double setInsulinFillGauge(double);
    void simulateIOBFluctuation();
    void simulateInsulinFillGaugeFluctuation();
    bool isGlucoseLevelSafe () const;
    double generateRandomDouble(double min = 30.0, double max = 60.0);
    bool userPaused = false;  // indicates a user-initiated pause
    
private:
    double* currentBloodGlucoseLevel;
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

};

#endif // CONTROL_IQ_H
