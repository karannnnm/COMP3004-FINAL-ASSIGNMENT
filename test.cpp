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
    manager.addProfile(Profile("Test Profile", 0.5, 2.0, 15.0, 6.0));
    
    BolusCalculator bolus(manager);
    bolus.getProfileByName("Test Profile");
    
    ControlIQ controlIQ;
    controlIQ.fetchBolusData(bolus);
    controlIQ.fetchCurrentProfile(bolus);
    controlIQ.linkCurrentBloodGlucoseLevel(bolus);
    controlIQ.setInsulinFillGauge(100.0);

    // Test with high glucose
    bolus.setCurrentGlucoseLevel(12.0);
    cout << "Initial glucose: " << controlIQ.getCurrentBloodGlucose() << " mmol/L" << endl;
    
    // Should trigger correction
    controlIQ.predictBolusRequired();

    controlIQ.getCurrentBloodGlucose();

    return 0;
}
