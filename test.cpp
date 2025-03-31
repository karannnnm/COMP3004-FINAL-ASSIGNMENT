#include "Profile.h"
#include "ProfileManager.h"
#include <iostream>
using namespace std;

int main() {

    Create a ProfileManager instance
    ProfileManager manager;

    // Test Case 1: Add profiles
    cout << "Adding profiles..." << endl;
    manager.addProfile(Profile("Morning Routine", 1.0, 2.0, 15.0, 100.0));
    manager.addProfile(Profile("Lunch Routine", 1.2, 2.5, 12.0, 110.0));
    manager.addProfile(Profile("Exercise Routine", 0.8, 1.8, 20.0, 90.0));
    manager.displayAllProfiles();

    // Test Case 2: Retrieve a profile
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

    return 0;
}