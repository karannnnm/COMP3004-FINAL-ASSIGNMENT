#ifndef BOLUS_CALCULATOR_H
#define BOLUS_CALCULATOR_H
using namespace std;

#include "ProfileManager.h"
// #include "ControlIQ.h" // Include the header file for ControlIQ
#include <vector>


/*
    BOLUS CALCULATION:

        {Input Parameters: 
            - Insulin to carb ratio(ICR)/ carbRatio : fetched from Profile
            - Correction factor                     : fetched from Profile
            - target blood glucose                  : fetched from Profile
            - current blood glucose (mmol/L)        : let user input manually now
            - insulin on board:                     : use default of 5units 
            - total carbs                           : let user input manually now
        }


        2.1; Food Bolus = carb bolus = total carbs/ICR
                for a meal of 50g carbs = carb bolus = 50/1 = 50 units. 

        
        2.2: Correction bolus = bolus = (current blood glucos(bg) - target gb)/correction factor
                For a target blood glucose of 5 mmol/L and a current BG of 7 mmol/L:
                    correction bolus = 7-5/1 = 2units
        
        2.3: Total required bolus = total bolus = carb bolus + correction bolus
                                = 50+2 = 52 units

        2.4: Adjustment for IOB = Final bolus = total bolus - IOB
                                              = 52-5 = 47 units

        
        3: Bolus distribution over 3 hours: 

            3.1: Immediate Bolus (60%) = Immediate bolus = 0.6 * final bolus
                                                        = 0.6 * 47 = 28.2 units

            3.2: Extended bolus (40% over 3 hours) = Extended bolus = 0.4 * final bolus
                                                                    = 0.4 * 47 = 18.8

            3.3: Bolus Rate per hour:
                since extended bolus is spread over 3 hours
                Bolus rate per hour = Extended bolus / 3 = 18.8/3 = 6.27 units every hour


        { OUTPUT
                4: Bolus plan:
                Immediate Insulin Dose: 28.2 units at meal time
                Extended Insulin delivery = 6.27 units per hour for 3 hours
                Total insulin adminstered = 47 units
        }
*/



class BolusCalculator {
public:

// Friend method to allow ControlIQ to access bolus data
friend std::vector<double> getBolusData(const BolusCalculator& bolusCalculator);


// Constructor
BolusCalculator(ProfileManager& profileManager);


//done
//step 1: load a profile
//fetch profile from ProfileManager and save it in currentProfile
void getProfileByName(const std::string& profileName);
Profile* currentProfile;




//done
//step 2: fetch variables for bolus calculation 
// 2.1 setters and getters for manual inputs for calculation
void setCurrentGlucoseLevel(double level);
void setTotalCarbs(double totalCarbs);
double getCurrentGlucoseLevel();
double getTotalCarbs();
double getIOB();

// 2.2 variables coming from Profile are saved in the
//  fetchedVarName variable private field which are set when profile is loaded 
//for debugging
void printStep2();

//this is to check if we have all variables ready for the bolus calculation i.e step 3
bool areInputsValid() const;


    




// step 3: Perform bolus calculation
//DONE!!
    // 3.1
        // Calculate bolus dose based on manual input
        // need these variables for the calculation ICR, targetBloodGlucose, correctionFactor, currentBloodGlucose, totalCarbs, IOB.
        double calculateBolus(); 
    

        //3.2
        //calculate how much bolus user needs immediately.
        void setPercentOfImmediateDose(double percent);
        void calculateImmediateBolusDose();
    
        //3.3
        //calculate how much bolus is left for extended delivery.
        void setDurationForExtendedBolus(int hours);
        
        //TO DO: 
        void setPercentOfExtendedDose(double percent);
        // Handles extended bolus (delayed insulin delivery)
        void calculateExtendedBolusDose();

        //for debugging
        void printStep3();
    
    
    
    
    // Allows user to override suggested bolus
    //allow user to ovveride the totalBolusAfterIOB.
    // for the immediateDose and extendedDose, use same duration and percent, but calculate dose based on overriden dose.
    void overrideBolus(double newDose);


    // Cancels ongoing bolus
    void cancelBolus();
    // Sends bolus data to ControlIQ system
    // void sendBolusDataToControlIQ(ControlIQ& controlIQ);
    

    private:
    ProfileManager& profileManager;  // Reference to access user settings
    
    // input variables for bolus calc that user will manually enter
    double currentBloodGlucoseLevel;
    double totalCarbs;
    
    //random IOB
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
