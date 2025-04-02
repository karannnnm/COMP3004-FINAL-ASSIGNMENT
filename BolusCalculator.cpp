#include "BolusCalculator.h"
using namespace std;

#include "ProfileManager.h"

// Constructor: Accepts a reference to ProfileManager so it can access stored profiles
BolusCalculator::BolusCalculator(ProfileManager& profileManager) : profileManager(profileManager) {}

//load profile into BolusCalculator class.

void BolusCalculator::getProfileByName(const std::string& profileName){

    //case1: if no profiles exist
    if (profileManager.isEmpty())
    {
        static Profile defaultProfile("Default Profile", 1.4, 1, 1, 5);
        currentProfile = &defaultProfile;
        fetchedICR = currentProfile->getCarbRatio();
        fetchedCorrectionFactor = currentProfile->getCorrectionFactor();
        fetchedTargetBloodGlucoseLevel = currentProfile->getTargetGlucose();
    }

    //case2: if profile exists with the input name
    else if(profileManager.getProfile(profileName)){
        currentProfile = profileManager.getProfile(profileName);
        fetchedICR = currentProfile->getCarbRatio();
        fetchedCorrectionFactor = currentProfile->getCorrectionFactor();
        fetchedTargetBloodGlucoseLevel = currentProfile->getTargetGlucose();
    }
    //case3: if no profile exists with the input name
    else{
        cout<<"couldnt load profile. (check BolusCalc.cpp ln 41 )."<<endl;
        cout<<"loading the default profile for now."<<endl;
        static Profile defaultProfile("Default Profile", 1.4, 1, 1, 5);
        currentProfile = &defaultProfile;
        fetchedICR = currentProfile->getCarbRatio();
        fetchedCorrectionFactor = currentProfile->getCorrectionFactor();
        fetchedTargetBloodGlucoseLevel = currentProfile->getTargetGlucose();
    }
}

void BolusCalculator::setCurrentGlucoseLevel(double level){
    this->currentBloodGlucoseLevel = level;
}

void BolusCalculator::setTotalCarbs(double totalCarbs){
    this->totalCarbs = totalCarbs;
}
double BolusCalculator::getCurrentGlucoseLevel(){
    return this->currentBloodGlucoseLevel;
}

double BolusCalculator::getTotalCarbs(){
    return this->totalCarbs;
}

double BolusCalculator::getIOB(){
    return this->IOB;
}

