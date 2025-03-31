#include "Profile.h"
#include <iostream>
using namespace std;

// Constructor
Profile::Profile(const std::string& name, double basalRate, double correctionFactor, double carbRatio, double targetGlucose)
    : name(name), 
      basalRate(basalRate), 
      correctionFactor(correctionFactor), 
      carbRatio(carbRatio), 
      targetGlucose(targetGlucose) {}

// Getters
std::string Profile::getName() const {
    return name;
}

double Profile::getBasalRate() const {
    return basalRate;
}

double Profile::getCorrectionFactor() const {
    return correctionFactor;
}

double Profile::getCarbRatio() const {
    return carbRatio;
}

double Profile::getTargetGlucose() const {
    return targetGlucose;
}

// Setters
void Profile::setBasalRate(double rate) {
    basalRate = rate;
}

void Profile::setCorrectionFactor(double factor) {
    correctionFactor = factor;
}

void Profile::setCarbRatio(double ratio) {
    carbRatio = ratio;
}

void Profile::setTargetGlucose(double glucose) {
    targetGlucose = glucose;
}
void Profile::setProfileType(string profileType){
    this->profileType = profileType;
}

// Display the current profile values
void Profile::displayProfile() const {
    cout << "Profile:" << endl;
    cout << "Name: " << name << endl;
    cout<<  "Profile Type: "<<profileType<<endl;
    cout << "Basal Rate: " << basalRate << " units/hour" << endl;
    cout << "Correction Factor: " << correctionFactor << " mg/dL per unit" << endl;
    cout << "Carb Ratio: " << carbRatio << " mg/dL per unit" << endl;
    cout << "Target Glucose: " << targetGlucose << " mg/dL" << endl;
}
