#ifndef CONTROL_IQ_H
#define CONTROL_IQ_H


#include <vector>

class Profile;
class BolusCalculator;


/*
    step1: fetch the correct profile and its bolus dose data from BolusCalculator.
    step2: fetch the currentBloodGlucose and decrease incrementally as the bolus is being recieved.
            - keep the duration of the bolus as well.
            - amount of currentBloodGlucose to be decreased is based on the correction factor.
            - For example: if 1 unit of insulin drops glucose by 2 mmol/L, and we deliver 5 units,
                    BG could theoretically drop by 10 mmol/L over a few hours (in ideal conditions).


*/





class ControlIQ {
public:
    // Method to fetch bolus data from BolusCalculator
    ControlIQ();

    //step1.
    void fetchBolusData(const BolusCalculator& bolusCalculator);
    void fetchCurrentProfile(const BolusCalculator& bolusCalculator);


    Profile* currentProfile = nullptr;
    
    
    
    double getCurrentBloodGlucose();
    
    
    
    
    // Method to display the received bolus data (for debugging)
    void displayBolusData() const;
    void displayProfileData() const;


private:
    double immediateDose = 0.0;
    double extendedDose = 0.0;
    double extendedDosePerHour = 0.0;
    double durationOfExtendedBolus = 0;
    double currentBloodGlucoseLevel;
};

#endif // CONTROL_IQ_H