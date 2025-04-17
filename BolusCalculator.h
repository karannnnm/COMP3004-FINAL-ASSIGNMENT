// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#ifndef BOLUS_CALCULATOR_H
#define BOLUS_CALCULATOR_H
using namespace std;

#include "ProfileManager.h"
#include "ControlIQ.h" // Include the header file for ControlIQ
#include <vector>

class BolusCalculator {
public:

    // Friend method to allow ControlIQ to access bolus data
    friend std::vector<double> getBolusData(const BolusCalculator& bolusCalculator);
    friend Profile* getCurrentProfile(const BolusCalculator& bolusCalculator);

    // Constructor
    BolusCalculator(ProfileManager& profileManager);
    void getProfileByName(const std::string& profileName); //fetch profile from ProfileManager and save it in currentProfile
    Profile* currentProfile = nullptr;

    // setters and getters for manual inputs for calculation
    void setCurrentGlucoseLevel(double level);
    void setTotalCarbs(double totalCarbs);
    double getCurrentGlucoseLevel() const;
    double& getCurrentGlucoseLevelToLink();
    double getTotalCarbs();

    double getIOB();

    //for debugging
    void printStep2();
    bool areInputsValid() const;

    double fetchCurrentGlucoseLevelFromCGM(); //for fetching from CGM
    double calculateBolus(); // Calculate bolus

    //calculate how much bolus user needs immediately.
    void setPercentOfImmediateDose(double percent);
    void calculateImmediateBolusDose();

    // Handles extended bolus (delayed insulin delivery)
    void setDurationForExtendedBolus(int hours);
    void setPercentOfExtendedDose(double percent);
    void calculateExtendedBolusDose();

    //for debugging
    void printStep3();
    
    // Allows user to override suggested bolus
    void overrideBolus(double newDose);

    // Cancels ongoing bolus
    void cancelBolus();

    double getTotalBolusAfterIOB();
    double getImmediateDose();
    double getExtendedDosePerHour();
    double fetchCarbsFromCGM();
    

private:
    ProfileManager& profileManager;  // Reference to access user settings
    
    // input variables for bolus calc that user will manually enter
    double currentBloodGlucoseLevel = 7.6;
    double totalCarbs = 50.0;
    double IOB = 5.0;

    //inputs variables for bolus calc coming from Profile
    double fetchedICR;
    double fetchedCorrectionFactor;
    double fetchedTargetBloodGlucoseLevel;


    double percentOfImmediateDose;
    double percentOfExtendedDose;
    double durationOfExtendedBolus;

    //outputs
    double totalBolusAfterIOB;
    double immediateDose;
    double extendedDose;
    double extendedDosePerHour;

};

#endif // BOLUS_CALCULATOR_H
