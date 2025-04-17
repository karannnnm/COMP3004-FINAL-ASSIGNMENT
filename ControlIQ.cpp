// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir
#include "ControlIQ.h"
#include <iostream>
#include "BolusCalculator.h"
#include "Profile.h"
#include <QDebug>
#include <random>

using namespace std;

ControlIQ::ControlIQ() {}


void ControlIQ::fetchBolusData(const BolusCalculator& bolusCalculator) {
    std::vector<double> bolusData = getBolusData(bolusCalculator);
    immediateDose = bolusData[0];
    extendedDose = bolusData[1];
    extendedDosePerHour = bolusData[2];
    durationOfExtendedBolus =bolusData[3];
    IOB = bolusData[4];
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
    } else {
        //on program startup --> use default profile
        static Profile defaultProfile("Default Profile", 1.4, 1, 1, 5);
        currentProfile = &defaultProfile;
        cout<<"Loaded the default profile. "<<endl;
    }
}

bool ControlIQ::isGlucoseLevelSafe () const{
    if (!currentBloodGlucoseLevel) return false;
    
    if (*currentBloodGlucoseLevel<3.9)
    {
        cout<<"\nBGLEVEL too low = "<<*currentBloodGlucoseLevel<<endl;
        cout<<"Suspending all insulin delivery !!!"<<endl;
        return false;
    }
    return true;
}


void ControlIQ::mimicGlucoseSpike() {
    double oldLevel = *currentBloodGlucoseLevel;
    *currentBloodGlucoseLevel += 0.105;
    
    cout << "Glucose increased from " << oldLevel << " to " << *currentBloodGlucoseLevel << " mmol/L" << endl;
    
    // If glucose has recovered above 3.9, indicate that insulin delivery can resume
    if (oldLevel < 3.9 && *currentBloodGlucoseLevel >= 3.9) {
        cout << " Glucose levels recovered - insulin delivery can resume" << endl;
    }
}


double ControlIQ::getCurrentBloodGlucose() {
    if (this->currentBloodGlucoseLevel) {
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

    if (!isGlucoseLevelSafe())
    {
        if (bolusStatus != BolusDeliveryStatus::PAUSED && bolusStatus != BolusDeliveryStatus::SUSPENDED)
        {
            pauseBolus();
        }
        return;
    }
  
    if (bolusStatus != BolusDeliveryStatus::NOT_STARTED && bolusStatus != BolusDeliveryStatus::PAUSED && bolusStatus != BolusDeliveryStatus::COMPLETED){
        qDebug()<<"Bolus seems suspended !!";
        return;
    }
    
    //deliver the immediate bolus dose
    if (bolusStatus == BolusDeliveryStatus::NOT_STARTED || bolusStatus == BolusDeliveryStatus::COMPLETED)
    { 
        //deliver immediate dose. decrease more glucose since its a big dose.
        qDebug()<<"Starting  immediate bolus";
        double glucoseLevelDrop = immediateDose*2.5*(currentProfile->getCorrectionFactor() / 100);
        *currentBloodGlucoseLevel -= glucoseLevelDrop;
        this->IOB += immediateDose;
        this->insulinFillGauge -= immediateDose;

        qDebug()<<"updated bg level after immediate bolus dose has been given = "<<*currentBloodGlucoseLevel;
        qDebug()<<"Immediate dose left to be administered = "<<immediateDose;

        emit immediateDoseDelivered();  // Notify mainwindow that the immediate dose has been delivered.
        
        // start delivering the extended dose.
        bolusDelivered = 0;
        totalBolusToDeliver = extendedDose;
        // cout<<totalBolusToDeliver<<endl;
        extendedDurationSeconds = durationOfExtendedBolus * 3600;   //convert duration to secs

        qDebug() << "Starting extended bolus:";
        qDebug() << "Total to deliver: " << totalBolusToDeliver << " units";
        qDebug() << "Duration: " << extendedDurationSeconds << " seconds";

    }
    immediateDose = 0;
    bolusStatus = BolusDeliveryStatus::RUNNING;
}


void ControlIQ::deliverExtendedBolus() {

    if (bolusStatus == BolusDeliveryStatus::NOT_STARTED || bolusStatus == BolusDeliveryStatus::COMPLETED)
    {
        qDebug()<<"[Extended Bolus] not started";
        return;
    }

    if (!isGlucoseLevelSafe()) {
        if (bolusStatus == BolusDeliveryStatus::RUNNING) {
            pauseBolus();
        }
        return;
    }
    else if (isGlucoseLevelSafe() && bolusStatus == BolusDeliveryStatus::PAUSED && !userPaused)
    {
        resumeBolus();
    }

    // Check if there's any extended bolus to deliver
    if (extendedDose <= 0 || durationOfExtendedBolus <= 0 || extendedDosePerHour <= 0) {
        qDebug() << "[Extended Bolus] No extended bolus remaining to deliver.";
        bolusStatus = BolusDeliveryStatus::COMPLETED;
        return;
    }

    // Check if bolus can be delivered based on status
    if (bolusStatus != BolusDeliveryStatus::RUNNING) {
        qDebug() << "Cannot deliver extended bolus. Status = " << static_cast<int>(bolusStatus);
        return;
    }

    // Calculate how much insulin to deliver in this 30-second interval
    double hoursPerInterval = 0.5; // 30 minutes = 0.5 hours
    double insulinToDeliver = extendedDosePerHour * hoursPerInterval;
    
    // Don't deliver more than what's remaining
    if (insulinToDeliver > extendedDose) {
        insulinToDeliver = extendedDose;
    }

    this->IOB += extendedDosePerHour/2;
    this->insulinFillGauge -= extendedDosePerHour/2;
    // deliver insulin and update remaining amount
    extendedDose -= insulinToDeliver;
    
    // calculate and apply glucose drop
    double glucoseDrop = insulinToDeliver * 1.5* (currentProfile->getCorrectionFactor() / 100);
    *currentBloodGlucoseLevel -= glucoseDrop;

    // reduce duration by 30 minutes (0.5 hours)
    durationOfExtendedBolus -= hoursPerInterval;

    
    qDebug() << "\n====== Extended Bolus Delivery Update ======";
    qDebug() << "Insulin delivered this interval: " << insulinToDeliver << " units";
    qDebug() << "Remaining extended dose: " << extendedDose << " units";
    qDebug() << "Glucose dropped by: " << glucoseDrop << " mmol/L";
    qDebug() << "Current glucose level: " << *currentBloodGlucoseLevel << " mmol/L";
    qDebug() << "Remaining duration: " << durationOfExtendedBolus << " hours";

    // if extended bolus is complete
    if (durationOfExtendedBolus <= 0 || extendedDose <= 0) {
        bolusStatus = BolusDeliveryStatus::COMPLETED;
        durationOfExtendedBolus = 0;
        extendedDose = 0;
        qDebug() << "[Extended Bolus] Delivery completed!";
        emit extendedDoseCompleted();  // Notify mainwindow that extended dose delivery is finished.
    }
}

void ControlIQ::pauseBolus(){
    if (bolusStatus != BolusDeliveryStatus::RUNNING)
    {
        qDebug() << "::::::::::::: Cannot pause bolus - current status: " << static_cast<int>(bolusStatus);
        return;
    }
    
    bolusStatus = BolusDeliveryStatus::PAUSED;
    qDebug()<<":::::::::::::::::: Bolus successfully paused !!";
}


void ControlIQ::resumeBolus(){

    if (!isGlucoseLevelSafe())
    {
        qDebug()<<"Cannot Resume Bolus - glucose level too low ";
        return;
    }
    

    if (bolusStatus != BolusDeliveryStatus::PAUSED)
    {
        qDebug() << ":::::::::  Cannot resume bolus - current status: " << static_cast<int>(bolusStatus);
        return;
    }
    userPaused = false;
    bolusStatus = BolusDeliveryStatus::RUNNING;
    qDebug()<<"::::::::::::::: Bolus successfully resumed";
}

void ControlIQ::suspendBolus() {
    if (bolusStatus == BolusDeliveryStatus::SUSPENDED) {
        qDebug() << ":::::::::: BOLUS IS ALREADY SUSPENDED!!";
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
// > 10	         Increase basal rate slightlys
void ControlIQ::deliverBasal(double insulinAmount){

    if (!isGlucoseLevelSafe()) {
        cout << "Basal delivery suspended - glucose level too low" << endl;
        return;
    }

    double glucoseToReduce = insulinAmount * 10 * this->currentProfile->getCorrectionFactor();
    *currentBloodGlucoseLevel -= glucoseToReduce;

    cout << "\n====== Basal Delivery Update ======" << endl;
    cout << "Basal insulin delivered: " << insulinAmount << " units" << endl;
    cout << "Glucose reduced by: " << glucoseToReduce << " mmol/L" << endl;
    cout << "New glucose level: " << *currentBloodGlucoseLevel << " mmol/L" << endl;
}


void ControlIQ::monitorGlucoseLevel() {
    double bg = *currentBloodGlucoseLevel;
    //qDebug("test 0");
    double baseRate = currentProfile->getBasalRate();   // units/hour
    //qDebug("test 0.5");
    double insulinPerInterval = baseRate / 120.0;      // For 30-second intervals (120 intervals per hour)
    //qDebug("test 1");
    cout << "\n====== Glucose Monitoring ======" << endl;
    cout << "Current BG: " << bg << " mmol/L" << endl;
    cout << "Base basal rate: " << baseRate << " units/hour" << endl;


    if (!isGlucoseLevelSafe()) {
        // Suspend all insulin delivery
        if (bolusStatus == BolusDeliveryStatus::RUNNING) {
            pauseBolus();
        }
        return;
    }

    // Determine how much basal to give based on glucose level
    if (bg < 3.9) {
        qDebug() << "BG too low - Suspending basal delivery";
        return;  // No insulin delivered
    } 
    else if (bg >= 3.9 && bg < 6.5) {
        qDebug() << "BG low - Reducing basal rate by 35%";
        deliverBasal(insulinPerInterval * 0.35);
    }
    else if (bg >= 6.5 && bg <= 10.0) {
        qDebug() << "BG normal - Delivering standard basal rate";
        deliverBasal(insulinPerInterval);
    }
    else { // bg > 10.0
        qDebug() << "BG high - Increasing basal rate by 25%";
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

double returnCurrentBloodGlucoseLevelToControlIQ(const ControlIQ& controlIQ) {
    if (controlIQ.currentBloodGlucoseLevel) {
        return *(controlIQ.currentBloodGlucoseLevel);
    } else {
        cout << "Error: currentBloodGlucoseLevel is not linked!" << endl;
        return -1;
    }
}

double ControlIQ::getIOB()
{
    return IOB;
}

double ControlIQ::getInsulinFillGauge()
{
    return insulinFillGauge;
}

void ControlIQ::setInsulinFillGauge(double insulinValue)
{
    insulinFillGauge = insulinValue;
}


BolusDeliveryStatus ControlIQ::getBolusStatus()
{
    return bolusStatus;
}

void ControlIQ::simulateIOBFluctuation() {
    if (bolusStatus == BolusDeliveryStatus::COMPLETED || bolusStatus == BolusDeliveryStatus::NOT_STARTED) {

        this->IOB = max(0.0, this->IOB - 0.5);
    }
    else if (bolusStatus == BolusDeliveryStatus::RUNNING) {

        this->IOB = max(0.0, this->IOB - 0.1);
    }
}

void ControlIQ::simulateInsulinFillGaugeFluctuation() {
    if (bolusStatus != BolusDeliveryStatus::RUNNING) {
        this->insulinFillGauge = max(0.0, this->insulinFillGauge - 1);
    }
}

double ControlIQ::generateRandomDouble(double min, double max) {
    // Use C++11 random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}


void ControlIQ::predictBolusRequired() {

    if (!currentBloodGlucoseLevel || !currentProfile) {
        qDebug() << "Error: Missing glucose data or profile";
        return;
    }

    // will terminate if a bolus is running or paused
    if (bolusStatus == BolusDeliveryStatus::RUNNING || 
        bolusStatus == BolusDeliveryStatus::PAUSED ) {
        qDebug() << "Cannot predict - bolus is active";
        return;
    }

    if (*currentBloodGlucoseLevel > 10.0) {
        qDebug() << "\n====== Auto Correction Bolus Check ======";
        qDebug() << "Blood glucose is high (" << *currentBloodGlucoseLevel << " mmol/L)";
        
        // calc the units of bolus to deliver
        double targetGlucose = currentProfile->getTargetGlucose();
        double correctionFactor = currentProfile->getCorrectionFactor();
        double suggestedBolus = (*currentBloodGlucoseLevel - targetGlucose) / correctionFactor;
        qDebug()<<"-----Suggested dose = "<<suggestedBolus;

        // suggested bolus cannot be more than 5 units (prevent over correction)
        suggestedBolus = min(suggestedBolus, 5.0);
        
        if (suggestedBolus > 0) {
            qDebug() << "Delivering correction bolus: " << suggestedBolus << " units";
            emit automaticBolusAdministered();
            deliverImmediateCorrectionBolus(suggestedBolus);
        }
    }
}


void ControlIQ::deliverImmediateCorrectionBolus(double insulinAmt) {
    if (insulinFillGauge < insulinAmt) {
        qDebug() << "ERROR: Not enough insulin in reservoir!";
        return;
    }

    // calc glucose drop
    double glucoseLevelDrop = insulinAmt *  (currentProfile->getCorrectionFactor());
    double oldGlucose = *currentBloodGlucoseLevel;
    
    qDebug()<<"Glucose level to drop = "<<glucoseLevelDrop;
    
    // deliver insulin and update values
    *currentBloodGlucoseLevel -= glucoseLevelDrop;
    this->IOB += insulinAmt;
    this->insulinFillGauge -= insulinAmt;
    
    qDebug() << "\n====== Correction Bolus Results ======";
    qDebug() << "Insulin delivered: " << insulinAmt << " units";
    qDebug() << "Glucose dropped from: " << oldGlucose << " to " << *currentBloodGlucoseLevel << " mmol/L";
    qDebug() << "New IOB: " << IOB << " units";
    qDebug() << "Remaining insulin: " << insulinFillGauge << " units";
}
