// Devanjali Das, Gurleen Bassali, Karan Modi, Zainab Jahangir

#include "ProfileManager.h"

// Add a new profile
void ProfileManager::addProfile(const Profile& profile) {
    profiles.push_back(profile);
}

// Get a profile by name
Profile* ProfileManager::getProfile(const string& name) {
    for (auto& profile : profiles) {
        if (profile.getName() == name) {
            return &profile;
        }
    }
    return nullptr; // Return nullptr if not found
}

// Update an existing profile
bool ProfileManager::updateProfile(const string& name, double basalRate, double correctionFactor, double carbRatio, double targetGlucose) {
    Profile* profile = getProfile(name);
    if (profile) {
        profile->setBasalRate(basalRate);
        profile->setCorrectionFactor(correctionFactor);
        profile->setCarbRatio(carbRatio);
        profile->setTargetGlucose(targetGlucose);
        return true;
    }
    return false; // Profile not found
}

// Delete a profile by name
bool ProfileManager::deleteProfile(const string& name) {
    for (auto it = profiles.begin(); it != profiles.end(); ++it) {
        if (it->getName() == name) {
            profiles.erase(it);
            return true;
        }
    }
    return false; // Profile not found
}


bool ProfileManager::isEmpty(){
    return profiles.empty();
}

// Display all profiles
void ProfileManager::displayAllProfiles() const {
    if (profiles.empty()) {
        cout << "No profiles available." << endl;
        return;
    }
    for (const auto& profile : profiles) {
        profile.displayProfile();
        cout << "------------------------" << endl<<endl;
    }
}

std::vector<Profile>& ProfileManager::getProfiles() {
    return profiles;
}
