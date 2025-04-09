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
    // cout<<*currentBloodGlucoseLevel<<endl;
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

void ControlIQ::mimicGlucoseSpike(){
    if (bolusStatus != BolusDeliveryStatus::RUNNING)
    {
        *currentBloodGlucoseLevel += 0.012;
        cout<<"Glucose spiked by "<<*currentBloodGlucoseLevel <<" mmol/L"<<endl;
        return;
    }
}


double ControlIQ::getCurrentBloodGlucose() {
    if (this->currentBloodGlucoseLevel) {
        // cout<<*currentBloodGlucoseLevel<<endl;
        return *(this->currentBloodGlucoseLevel);
    } else {
        cout << "Error: currentBloodGlucoseLevel is not linked!" << endl;
        return -1;
    }
}

double ControlIQ::getDurationOfExtendedBolus(){
    return this->durationOfExtendedBolus;
}
     
    
void ControlIQ::startBolus(){
    if (bolusStatus != BolusDeliveryStatus::NOT_STARTED && bolusStatus != BolusDeliveryStatus::PAUSED){
        cout<<"Bolus seems suspended !!"<<endl;
        return;
    }
    
    //deliver the immediate bolus dose
    if (bolusStatus == BolusDeliveryStatus::NOT_STARTED)
    { 
        //deliver immediate dose. decrease more glucose since its a big dose.
        double glucoseLevelDrop = immediateDose*2.5*(currentProfile->getCorrectionFactor() / 100);
        // cout<<glucoseLevelDrop<<endl;
        *currentBloodGlucoseLevel -= glucoseLevelDrop;
        cout<<"updated bg level after immediate bolus dose has been given = "<<*currentBloodGlucoseLevel<<endl;
        cout<<"Immediate dose left to be administered = "<<immediateDose<<endl;
        
        
        // start delivering the extended dose.
        bolusDelivered  =0;
        totalBolusToDeliver = extendedDose;
        // cout<<totalBolusToDeliver<<endl;
        extendedDurationSeconds = durationOfExtendedBolus * 3600;   //convert duration to secs

        cout << "Starting extended bolus:" << endl;
        cout << "Total to deliver: " << totalBolusToDeliver << " units" << endl;
        cout << "Duration: " << extendedDurationSeconds << " seconds" << endl;

    }
    immediateDose = 0;
    bolusStatus = BolusDeliveryStatus::RUNNING;
}


void ControlIQ::deliverExtendedBolus() {
    // Check if there's any extended bolus to deliver
    if (extendedDose <= 0 || durationOfExtendedBolus <= 0 || extendedDosePerHour <= 0) {
        cout << "[Extended Bolus] No extended bolus remaining to deliver." << endl;
        bolusStatus = BolusDeliveryStatus::COMPLETED;
        return;
    }

    // Check if bolus can be delivered based on status
    if (bolusStatus != BolusDeliveryStatus::RUNNING) {
        cout << "Cannot deliver extended bolus. Status = " << static_cast<int>(bolusStatus) << endl;
        return;
    }

    // Calculate how much insulin to deliver in this 30-second interval
    double hoursPerInterval = 0.5; // 30 minutes = 0.5 hours
    double insulinToDeliver = extendedDosePerHour * hoursPerInterval;
    
    // Don't deliver more than what's remaining
    if (insulinToDeliver > extendedDose) {
        insulinToDeliver = extendedDose;
    }

    // deliver insulin and update remaining amount
    extendedDose -= insulinToDeliver;
    
    // calculate and apply glucose drop
    double glucoseDrop = insulinToDeliver * 1.5* (currentProfile->getCorrectionFactor() / 100);
    *currentBloodGlucoseLevel -= glucoseDrop;

    // reduce duration by 30 minutes (0.5 hours)
    durationOfExtendedBolus -= hoursPerInterval;

    
    cout << "\n====== Extended Bolus Delivery Update ======" << endl;
    cout << "Insulin delivered this interval: " << insulinToDeliver << " units" << endl;
    cout << "Remaining extended dose: " << extendedDose << " units" << endl;
    cout << "Glucose dropped by: " << glucoseDrop << " mmol/L" << endl;
    cout << "Current glucose level: " << *currentBloodGlucoseLevel << " mmol/L" << endl;
    cout << "Remaining duration: " << durationOfExtendedBolus << " hours" << endl;

    //  if extended bolus is complete
    if (durationOfExtendedBolus <= 0 || extendedDose <= 0) {
        bolusStatus = BolusDeliveryStatus::COMPLETED;
        durationOfExtendedBolus = 0;
        extendedDose = 0;
        cout << "\n[Extended Bolus] Delivery completed!" << endl;
    }
}

void ControlIQ::pauseBolus(){
    if (bolusStatus != BolusDeliveryStatus::RUNNING)
    {
        cout << "::::::::::::: Cannot pause bolus - current status: " << static_cast<int>(bolusStatus) << endl;
        return;
    }
    
    bolusStatus = BolusDeliveryStatus::PAUSED;
    cout<<":::::::::::::::::: Bolus successfully paused !!"<<endl;
}


void ControlIQ::resumeBolus(){
    if (bolusStatus != BolusDeliveryStatus::PAUSED)
    {
        cout << ":::::::::  Cannot resume bolus - current status: " << static_cast<int>(bolusStatus) << endl;
        return;
    }
    bolusStatus = BolusDeliveryStatus::RUNNING;
    cout<<"::::::::::::::: Bolus successfully resumed"<<endl;       
}

void ControlIQ::suspendBolus() {
    if (bolusStatus == BolusDeliveryStatus::SUSPENDED) {
        cout << ":::::::::: BOLUS IS ALREADY SUSPENDED!!" << endl;
        return;
    }
    
    // Change status to suspended
    bolusStatus = BolusDeliveryStatus::SUSPENDED;
    
    // Reset all bolus-related variables
    immediateDose = 0;
    extendedDose = 0;
    extendedDosePerHour = 0;
    durationOfExtendedBolus = 0;
    bolusDelivered = 0;
    totalBolusToDeliver = 0;
    extendedDurationSeconds = 0;
    
    cout << "\n====== BOLUS SUSPENDED ======" << endl;
    cout << "All bolus delivery values have been reset" << endl;
    displayBolusData(); // Show the reset values
}
    



// < 3.9	     Suspend basal delivery
// 3.9 - 6.5	 Lower basal rate slightly
// 6.5 - 10	     Normal basal delivery
// > 10	         Increase basal rate slightly

void ControlIQ::deliverBasal(double insulinAmount){
    double glucoseToReduce = insulinAmount * this->currentProfile->getCorrectionFactor();
    *currentBloodGlucoseLevel -= glucoseToReduce;
    

    cout << "\n====== Basal Delivery Update ======" << endl;
    cout << "Basal insulin delivered: " << insulinAmount << " units" << endl;
    cout << "Glucose reduced by: " << glucoseToReduce << " mmol/L" << endl;
    cout << "New glucose level: " << *currentBloodGlucoseLevel << " mmol/L" << endl;
}


void ControlIQ::moniterGlucoseLevel() {
    double bg = *currentBloodGlucoseLevel;
    double baseRate = currentProfile->getBasalRate();   // units/hour
    double insulinPerInterval = baseRate / 120.0;      // For 30-second intervals (120 intervals per hour)

    cout << "\n====== Glucose Monitoring ======" << endl;
    cout << "Current BG: " << bg << " mmol/L" << endl;
    cout << "Base basal rate: " << baseRate << " units/hour" << endl;

    // Determine how much basal to give based on glucose level
    if (bg < 3.9) {
        cout << "BG too low - Suspending basal delivery" << endl;
        return;  // No insulin delivered
    } 
    else if (bg >= 3.9 && bg < 6.5) {
        cout << "BG low - Reducing basal rate by 35%" << endl;
        deliverBasal(insulinPerInterval * 0.65);
    }
    else if (bg >= 6.5 && bg <= 10.0) {
        cout << "BG normal - Delivering standard basal rate" << endl;
        deliverBasal(insulinPerInterval);
    }
    else { // bg > 10.0
        cout << "BG high - Increasing basal rate by 25%" << endl;
        deliverBasal(insulinPerInterval * 1.25);
    }
}



void ControlIQ::displayBolusData() const {
    cout << endl<<"ControlIQ Bolus Data:" << endl;
    cout << "Immediate Dose: " << immediateDose << " units" << endl;
    cout << "Extended Dose: " << extendedDose << " units" << endl;
    cout << "Extended Dose Per Hour: " << extendedDosePerHour << " units/hour" << endl;
    cout << "Duration of Extended Bolus: " << durationOfExtendedBolus << " hours" << endl;
    if (currentBloodGlucoseLevel) {
        cout << "Current blood glucose level = " << *currentBloodGlucoseLevel << " mmol/L" << endl;
    } else {
        cout << "Current blood glucose level is not linked!" << endl;
    }
   

}
    
void ControlIQ::displayProfileData() const{

    cout<<"Details of the fetched profile from Bolus: "<<endl<<endl;
    this->currentProfile->displayProfile();
    cout<<endl<<endl;
    
}