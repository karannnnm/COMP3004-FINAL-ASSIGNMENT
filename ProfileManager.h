#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include "Profile.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class ProfileManager {
public:
    // Add a new profile
    void addProfile(const Profile& profile);

    // Get a profile by name
    Profile* getProfile(const string& name);

    // Update an existing profile
    bool updateProfile(const string& name, double basalRate, double correctionFactor, double carbRatio, double targetGlucose);

    // Delete a profile by name
    bool deleteProfile(const string& name);

    //returns true if list is empty
    bool isEmpty();
    
    // Display all profiles
    void displayAllProfiles() const;

    std::vector<Profile>& getProfiles();


private:
    vector<Profile> profiles; // List of profiles
};

#endif // PROFILE_MANAGER_H
