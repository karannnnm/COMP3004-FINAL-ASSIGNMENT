#include "Profile.h"
#include "ProfileManager.h"
#include "BolusCalculator.h"
#include "ControlIQ.h"
#include "CGM.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main() {

    // Create a ProfileManager instance
    
    // Test Case 1: Add profiles
    // cout << "Adding profiles..." << endl;
    // Profile::Profile(const std::string &name, double basalRate, double correctionFactor, double carbRatio, double targetGlucose)
    ProfileManager manager;
    manager.addProfile(Profile("Lunch Routine", 0.23, 1.1, 2, 7.9));
    manager.addProfile(Profile("Morning Routine", 0.67, 1.23, 15.0, 6.7));
    manager.addProfile(Profile("Exercise Routine", 0.72, 0.96, 20.0, 7));
    manager.addProfile(Profile("Bolus Test",0.45,1.11,1,5.7));
    manager.addProfile(Profile("ControlIQ test",0.92,1,1,5.5));

    // manager.displayAllProfiles();



    //testing for ControlIQ class
    BolusCalculator bolus(manager);
    bolus.getProfileByName("ControlIQ test");

    bolus.setCurrentGlucoseLevel(6.4);
    bolus.setTotalCarbs(50);
    bolus.calculateBolus();

    bolus.setPercentOfImmediateDose(50); 
    bolus.calculateImmediateBolusDose();

    bolus.setPercentOfExtendedDose(50);
    bolus.setDurationForExtendedBolus(4);
    bolus.calculateExtendedBolusDose();
    // bolus.printStep2();
    // bolus.areInputsValid();
    // bolus.printStep3();


    // cout<<bolus.getCurrentGlucoseLevel()<<endl;    
    
    ControlIQ controlIQ;
    controlIQ.fetchBolusData(bolus);
    controlIQ.fetchCurrentProfile(bolus);
    controlIQ.linkCurrentBloodGlucoseLevel(bolus);

    CGM cgm;
    cgm.linkToControlIQ(controlIQ);

    // Initial CGM reading
    cout << "\n=== Initial State ===" << endl;
    cgm.updateGlucoseReading();
    cout << "Initial CGM Reading: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    // Start bolus and get first reading
    cout << "\n=== Starting Bolus ===" << endl;
    controlIQ.startBolus();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cgm.updateGlucoseReading();
    cout << "CGM Reading after bolus start: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    // First extended bolus delivery and reading
    cout << "\n=== First Extended Bolus Delivery ===" << endl;
    controlIQ.deliverExtendedBolus();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cgm.updateGlucoseReading();
    cout << "CGM Reading after first extended delivery: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    // Simulate glucose spike and get reading
    cout << "\n=== Simulating Glucose Spike ===" << endl;
    controlIQ.mimicGlucoseSpike();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cgm.updateGlucoseReading();
    cout << "CGM Reading after glucose spike: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    // Second extended bolus delivery and reading
    cout << "\n=== Second Extended Bolus Delivery ===" << endl;
    controlIQ.deliverExtendedBolus();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    cgm.updateGlucoseReading();
    cout << "CGM Reading after second extended delivery: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    cgm.disconnectCGM();
    cgm.linkToControlIQ(controlIQ);

    // Final monitoring
    cout << "\n=== Final Glucose Monitoring ===" << endl;
    controlIQ.monitorGlucoseLevel();
    cgm.updateGlucoseReading();
    cout << "Final CGM Reading: " << cgm.getCurrentGlucose() << " mmol/L" << endl;

    // Display glucose history
    cout << "\n=== CGM Reading History ===" << endl;
    cgm.getGlucoseHistory();

    return 0;
}
