#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <iostream>
using namespace std;



//input variables in this class: basal rates, carb ratio, correction factors and target glucose levels.
class Profile {
public:
    // Constructor
    Profile(const string& name, double basalRate, double correctionFactor, double carbRatio, double targetGlucose);

    // Getters
    string getName() const;
    double getBasalRate() const;
    double getCorrectionFactor() const;
    double getCarbRatio() const;
    double getTargetGlucose() const;

    // Setters
    void setBasalRate(double rate);
    void setCorrectionFactor(double factor);
    void setCarbRatio(double ratio);
    void setTargetGlucose(double glucose);
    void setProfileType(string profileType);
    // Display Function (for debugging)
    void displayProfile() const;

private:
    string name;              // User's name
    double basalRate;              // Basal insulin rate (units/hour)
    double correctionFactor;       // Correction factor (mg/dL per unit)
    double carbRatio;
    double targetGlucose;
};

#endif // PROFILE_H
