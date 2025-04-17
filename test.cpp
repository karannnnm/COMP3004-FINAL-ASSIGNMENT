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

    
   ProfileManager manager;
    cout<<manager.isEmpty()<<endl;
    // Test Case 1: Add profiles
    // cout << "Adding profiles..." << endl;
    manager.addProfile(Profile("Lunch Routine", 1.2, 2.5, 12.0, 110.0));
    manager.addProfile(Profile("Morning Routine", 1.0, 2.0, 15.0, 100.0));
    manager.addProfile(Profile("Exercise Routine", 0.8, 1.8, 20.0, 90.0));

    manager.displayAllProfiles();
    

    cout<<"Testing for bolus  -----------------------------------"<<endl;
    BolusCalculator bolusTest(manager);
    bolusTest.getProfileByName("Morning Routine");
    bolusTest.currentProfile->displayProfile();

    bolusTest.setCurrentGlucoseLevel(2.2);
    cout<<bolusTest.getCurrentGlucoseLevel();



    //testing for ProfileManager class
    /*
    Test Case 2: Retrieve a profile
    cout << "\nRetrieving 'Lunch Routine' profile..." << endl;
    Profile* lunchProfile = manager.getProfile("Lunch Routine");
    if (lunchProfile) {
        lunchProfile->displayProfile();
    } else {
        cout << "Profile not found!" << endl;
    }

    // Test Case 3: Update a profile
    cout << "\nUpdating 'Exercise Routine' profile..." << endl;
    if (manager.updateProfile("Exercise Routine", 0.9, 2.0, 18.0, 95.0)) {
        cout << "Profile updated successfully!" << endl;
    } else {
        cout << "Failed to update profile!" << endl;
    }

    manager.displayAllProfiles();



    // Test Case 4: Delete a profile
    cout << "\nDeleting 'Morning Routine' profile..." << endl;
    if (manager.deleteProfile("Morning Routine")) {
        cout << "Profile deleted successfully!" << endl;
    } else {
        cout << "Failed to delete profile!" << endl;
    }
    manager.displayAllProfiles();

    // Test Case 5: Attempt to retrieve a non-existent profile
    cout << "\nRetrieving 'Non-Existent Routine' profile..." << endl;
    Profile* nonExistentProfile = manager.getProfile("Non-Existent Routine");
    if (nonExistentProfile) {
        nonExistentProfile->displayProfile();
    } else {
        cout << "Profile not found!" << endl;
    }
    */   

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
