#include "Profile.h"
#include "ProfileManager.h"
#include "BolusCalculator.h"
#include "ControlIQ.h"
#include <iostream>
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

    bolus.setCurrentGlucoseLevel(3.89999);
    bolus.setTotalCarbs(50);
    bolus.calculateBolus();

    bolus.setPercentOfImmediateDose(50); 
    bolus.calculateImmediateBolusDose();

    bolus.setPercentOfExtendedDose(50);
    bolus.setDurationForExtendedBolus(4);
    bolus.calculateExtendedBolusDose();
    bolus.printStep2();
    bolus.areInputsValid();
    bolus.printStep3();


    // cout<<bolus.getCurrentGlucoseLevel()<<endl;    
    
    ControlIQ controlIQ;
    controlIQ.fetchBolusData(bolus);
    controlIQ.fetchCurrentProfile(bolus);
    controlIQ.linkCurrentBloodGlucoseLevel(bolus);



    controlIQ.displayBolusData();
    cout<<"\n--------------Starting bolus delivery -----------------------"<<endl<<endl;
    controlIQ.startBolus();
    controlIQ.mimicGlucoseSpike();
    
    controlIQ.displayBolusData();
    controlIQ.deliverExtendedBolus();
    controlIQ.deliverExtendedBolus();
    // cout<<"---------------bolus paused"<<endl;
    // controlIQ.pauseBolus();
    // cout<<"---------------bolus resumed"<<endl;
    // controlIQ.resumeBolus();
    controlIQ.deliverExtendedBolus();
    controlIQ.deliverExtendedBolus();
    // bolus.setCurrentGlucoseLevel(5.6);
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.deliverExtendedBolus();
    controlIQ.deliverExtendedBolus();
    controlIQ.deliverExtendedBolus();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();
    controlIQ.deliverExtendedBolus();
    controlIQ.deliverExtendedBolus();
    controlIQ.mimicGlucoseSpike();
    controlIQ.mimicGlucoseSpike();

    controlIQ.monitorGlucoseLevel();
    



    //    cout<<controlIQ.getDurationOfExtendedBolus()<<endl;



    // controlIQ.displayBolusData();
    




    // Testing for BolusCalculator class
    /*

    BolusCalculator bolusTest(manager);
    bolusTest.getProfileByName("Lunch Routine");
    bolusTest.setCurrentGlucoseLevel(11);
    bolusTest.setTotalCarbs(50);
    
    
    // bolusTest.printStep2();
    // cout<<bolusTest.areInputsValid()<<endl;
    // cout<<bolusTest.calculateBolus()<<endl;
    bolusTest.calculateBolus();
    bolusTest.setPercentOfImmediateDose(60);
    bolusTest.calculateImmediateBolusDose();    
    bolusTest.setDurationForExtendedBolus(3);
    bolusTest.setPercentOfExtendedDose(40);
    bolusTest.calculateExtendedBolusDose();
    cout<<endl<<endl<<endl;
    
    
    bolusTest.printStep3();
    
    cout<<endl<<endl<<endl;

    //manually overriding the bolus.
    bolusTest.overrideBolus(25);
    bolusTest.printStep3();

    bolusTest.cancelBolus();
    bolusTest.printStep3();
    */


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

    return 0;
}
