// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

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

double BolusCalculator::fetchCurrentGlucoseLevelFromCGM(){
    return this->currentBloodGlucoseLevel;
}


void BolusCalculator::setTotalCarbs(double totalCarbs){
    this->totalCarbs = totalCarbs;
}
double BolusCalculator::getCurrentGlucoseLevel()const{
    return this->currentBloodGlucoseLevel;
}

double& BolusCalculator::getCurrentGlucoseLevelToLink(){
    return this->currentBloodGlucoseLevel;
}

double BolusCalculator::getTotalCarbs(){
    return this->totalCarbs;
}

double BolusCalculator::getIOB(){
    return this->IOB;
}

bool BolusCalculator::areInputsValid() const{
    return (fetchedICR >0 && fetchedCorrectionFactor>0 && fetchedTargetBloodGlucoseLevel>0 && currentBloodGlucoseLevel>0 && totalCarbs>0 && IOB>0);
}

void BolusCalculator::printStep2(){
    cout<<"DO WE HAVE ALL 6 VARIABLES FOR BOLUS CALCULATION???????????"<<endl<<endl;
    cout<<"fetched ICR = "<<fetchedICR<<endl;
    cout<<"fetched CorrectionFactor = "<<fetchedCorrectionFactor<<endl;
    cout<<"fetched TargetBloodGlucoseLevel = "<<fetchedTargetBloodGlucoseLevel<<endl;
    cout<<"currentBloodGlucoseLevel= "<<currentBloodGlucoseLevel<<endl;
    cout<<"totalCarbs= "<<totalCarbs<<endl;
    cout<<"IOB = "<<IOB<<endl;

}

double BolusCalculator::calculateBolus(){
    if (areInputsValid())
    {
        
        //2.1
        double carbBolus = (this->totalCarbs)/fetchedICR;
        
        //2.2
        double correctionBolus = (currentBloodGlucoseLevel - fetchedTargetBloodGlucoseLevel)/fetchedCorrectionFactor;

        double totalRequiredBolusBeforeIOB = carbBolus + correctionBolus;

        this->totalBolusAfterIOB = totalRequiredBolusBeforeIOB - IOB;

        // cout<<"total bolus calculated after IOB = "<<totalBolusAfterIOB<<endl;
        return totalBolusAfterIOB;
    }
    else{
        // cout<<" one or more variables needed for the calculation are missing"<<endl;
        this->totalBolusAfterIOB = 0;
        return totalBolusAfterIOB;
    }
}



//extended bolus delivery calculation.

void BolusCalculator::setPercentOfImmediateDose(double percent){
    if (percent<0 || percent>100)
    {
        this->percentOfImmediateDose = 0;
        // cout<<"please enter a number between 0 and 100"<<endl;
        return;
    }
    
    else{
        this->percentOfImmediateDose = percent/100;
        // cout <<"Percent of immediate dose = "<<percentOfImmediateDose<<endl;
    }
}


void BolusCalculator::calculateImmediateBolusDose(){
    if (this->percentOfImmediateDose<=0 || this->percentOfImmediateDose>1)
    {   
        // cout<<"immediate dose is set to 0 units due to invalid percent of dose"<<endl;
        this->immediateDose = 0;
    }
    else{
        this->immediateDose = this->percentOfImmediateDose * this->totalBolusAfterIOB;
        // cout<<"Total immediate dose to be delivered = "<<immediateDose<<endl;
    }    
}



void BolusCalculator::setDurationForExtendedBolus(int hours){
    if (hours<=0 || hours>6)
    {
        // cout<<"hours has to be an integer between 0 and 6. set to 1 (default)"<<endl;
        this->durationOfExtendedBolus = 1;
        return;
    }
    else{
        this->durationOfExtendedBolus = hours;
        // cout<<"Duration of Extended Bolus = "<<durationOfExtendedBolus<<endl;
    }    
}



void BolusCalculator::setPercentOfExtendedDose(double percent){
    if (percent < 0 || percent > 100 )
    {
        // cout<<" could not set percent for extended dose. check percent for immediate dose"<<endl;
        this->percentOfExtendedDose = 0;
    }
    else{
        this->percentOfExtendedDose = percent/100;
        // cout<<"Percent of extended dose = "<<percentOfExtendedDose<<endl;
    }
    
}

void BolusCalculator::calculateExtendedBolusDose(){
    if(durationOfExtendedBolus <=0){
        // cout<<"duration of extended bolus is invalid"<<endl;
        return;
    }
    else{
        this->extendedDose = this->percentOfExtendedDose * this->totalBolusAfterIOB;
        this->extendedDosePerHour = this->extendedDose/durationOfExtendedBolus;
        // cout<<"Extended bolus dose = "<<extendedDose<<endl;
        // cout<<"Duration of extended delivery = "<<durationOfExtendedBolus<<endl;
        // cout<<"Dose per hour = "<<extendedDosePerHour<<endl;
        return;
    }

}


void BolusCalculator::overrideBolus(double newDose){
    totalBolusAfterIOB = newDose;
    calculateImmediateBolusDose();
    calculateExtendedBolusDose();

}

void BolusCalculator::cancelBolus(){
    cout<<"Cancelling bolus dose!!!!"<<endl;
    this->totalBolusAfterIOB = 0;
    this->immediateDose=0;
    this->extendedDose=0;
    this->extendedDosePerHour=0;
    this->durationOfExtendedBolus=0;
    this->percentOfImmediateDose=0;
    this->percentOfExtendedDose=0;
}


void BolusCalculator::printStep3(){
    cout<<"< <<<<<< Full Details about the bolus delivery >>>> >>>>>>>>"<<endl<<endl;
    cout<<"- Suggested bolus dose to be delivered= "<<totalBolusAfterIOB<<endl<<endl;

    cout<<"- Details about the immediate dose :"<<endl;
    cout<<"     percentage of immediate dose = "<<percentOfImmediateDose<<endl;
    cout<<"     Immediate bolus dose to be delivered = "<<immediateDose<<endl<<endl;

    cout<<"- Details about the extended dose : "<<endl<<endl;
    cout<<"     Percent of extended Dose = "<<percentOfExtendedDose<<endl;
    cout<<"     Duration of extended bolus =  "<<durationOfExtendedBolus<<endl;
    cout<<"     Extended bolus dose over "<<durationOfExtendedBolus<<" hours = "<<extendedDose<<endl;
    cout<<"     Amount of bolus delivered per hour = "<<extendedDosePerHour<<endl;
    
}

// Friend method to return bolus data
std::vector<double> getBolusData(const BolusCalculator& bolusCalculator) {
    return {
        bolusCalculator.immediateDose,
        bolusCalculator.extendedDose,
        bolusCalculator.extendedDosePerHour,
        bolusCalculator.durationOfExtendedBolus,
        // bolusCalculator.currentBloodGlucoseLevel
    };
}


double BolusCalculator::getTotalBolusAfterIOB()
{
    return totalBolusAfterIOB;
}

double BolusCalculator::getImmediateDose()
{
    return immediateDose;
}

double BolusCalculator::getExtendedDosePerHour()
{
    return extendedDosePerHour;
}

Profile* getCurrentProfile(const BolusCalculator& bolusCalculator){
    return bolusCalculator.currentProfile;
};
