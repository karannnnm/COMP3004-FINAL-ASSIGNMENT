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
    manager.addProfile(Profile("Lunch Routine", 1.2, 2.5, 2, 7.9));
    manager.addProfile(Profile("Morning Routine", 1.0, 2.0, 15.0, 100.0));
    manager.addProfile(Profile("Exercise Routine", 0.8, 1.8, 20.0, 90.0));
    manager.addProfile(Profile("Bolus Test",2,1,1,5));
    manager.addProfile(Profile("ControlIQ test",2,1,1,5));

    manager.displayAllProfiles();



    //testing for ControlIQ class
    BolusCalculator bolus(manager);
    bolus.getProfileByName("ControlIQ test");
    bolus.currentProfile->displayProfile();
    bolus.currentProfile->setCarbRatio(2);
    cout<<"after changing carb ratio<>>>>"<<endl;
    manager.displayAllProfiles();


    bolus.setCurrentGlucoseLevel(6.5);
    bolus.setTotalCarbs(50);
    bolus.calculateBolus();

    bolus.setPercentOfImmediateDose(50);
    bolus.calculateImmediateBolusDose();

    bolus.setPercentOfExtendedDose(50);
    bolus.setDurationForExtendedBolus(4);
    bolus.calculateExtendedBolusDose();
    
    
    // ControlIQ controlIQ;
    // controlIQ.fetchBolusData(bolus);
    // controlIQ.fetchCurrentProfile(bolus);
    // controlIQ.displayBolusData();
    // cout<<"---"<<endl;
    // cout<<controlIQ.getCurrentBloodGlucose()<<endl;
    // cout<<"---"<<endl;
    // controlIQ.displayBolusData();

    // bolus.printStep2();

    
    // controlIQ.displayProfileData();
    //to check whether whether controlIQ is pointing to the same profile as bolus
    // cout<<bolus.currentProfile<<endl;
    // cout<<controlIQ.currentProfile<<endl;





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