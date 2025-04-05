#include "ControlIQ.h"
#include <iostream>
#include "BolusCalculator.h"
#include "Profile.h"
using namespace std;

ControlIQ::ControlIQ() {}


void ControlIQ::fetchBolusData(const BolusCalculator& bolusCalculator) {
    std::vector<double> bolusData = getBolusData(bolusCalculator);
    immediateDose = bolusData[0];
    extendedDose = bolusData[1];
    extendedDosePerHour = bolusData[2];
    durationOfExtendedBolus =bolusData[3];
    // currentBloodGlucoseLevel = &bolusData[4];
}

void ControlIQ::linkCurrentBloodGlucoseLevel(BolusCalculator& bolusCalculator){
    this->currentBloodGlucoseLevel = &(bolusCalculator.getCurrentGlucoseLevelToLink());
}


void ControlIQ::fetchCurrentProfile(const BolusCalculator& bolusCalculator){

    Profile* profile = getCurrentProfile(bolusCalculator);
    if (profile)
    {
        this->currentProfile = profile;
    }else{
        cout<<"Couldnt load profile from the Bolus class"<<endl;
    }
    
    
}

void ControlIQ::displayProfileData() const{

    cout<<"Details of the fetched profile from Bolus: "<<endl<<endl;
    this->currentProfile->displayProfile();
    cout<<endl<<endl;

}

double ControlIQ::getCurrentBloodGlucose(){
    
    return *(this->currentBloodGlucoseLevel);
}

void ControlIQ::adjustCurrentBloodGlucose(){
    *currentBloodGlucoseLevel -= 1;
}



void ControlIQ::displayBolusData() const {
    cout << endl<<"ControlIQ Bolus Data:" << endl;
    cout << "Immediate Dose: " << immediateDose << " units" << endl;
    cout << "Extended Dose: " << extendedDose << " units" << endl;
    cout << "Extended Dose Per Hour: " << extendedDosePerHour << " units/hour" << endl;
    cout << "Duration of Extended Bolus: " << durationOfExtendedBolus << " hours" << endl;
    if (currentBloodGlucoseLevel) {
        cout << "Current blood glucose level = " << *currentBloodGlucoseLevel << " mmol/L" << endl << endl;
    } else {
        cout << "Current blood glucose level is not linked!" << endl << endl;
    }
}