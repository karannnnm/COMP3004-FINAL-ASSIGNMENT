#include "ControlIQ.h"
#include "CGM.h"
#include <iostream>
using namespace std;

CGM::CGM() : controlIQPtr(nullptr) {}

void CGM::linkToControlIQ(const ControlIQ& controlIQ) {
    controlIQPtr = &controlIQ;
    std::cout << "CGM linked to ControlIQ" << std::endl;
}

void CGM::updateGlucoseReading() {
    if (!controlIQPtr) {
        std::cout << "Error: CGM not connected to ControlIQ!" << std::endl;
        return;
    }

    // Get current glucose level from ControlIQ using friend function
    double currentGlucose = returnCurrentBloodGlucoseLevelToControlIQ(*controlIQPtr);
    
    // Store reading with timestamp
    time_t now = time(nullptr);
    glucoseHistory.push_back({now, currentGlucose});
    
    std::cout << "CGM Reading Updated - Current Glucose: " << currentGlucose << " mmol/L" << std::endl;
}

double CGM::getCurrentGlucose() const {
    if (!controlIQPtr) {
        cout << "Error: CGM not connected to ControlIQ!" << endl;
        return -1;
    }
    if (glucoseHistory.empty()) {
        return -1;
    }
    return glucoseHistory.back().second;
}

//storing pairs of time and glucose value in a vector...
const std::vector<std::pair<time_t, double>>& CGM::getGlucoseHistory() const {
    if (!controlIQPtr) {
        cout << "Error: CGM not connected to ControlIQ!" << endl;
        static const std::vector<std::pair<time_t, double>> emptyHistory;
        return emptyHistory;
    }
    if (glucoseHistory.empty()) {
        cout << "No glucose history available." << endl;
        static const std::vector<std::pair<time_t, double>> emptyHistory;
        return emptyHistory;
    }
    for (const auto& reading : glucoseHistory) {
        cout << "Time: " << std::ctime(&reading.first) 
             << "Glucose: " << reading.second << " mmol/L" << endl;
    }
    return glucoseHistory;
}


void CGM::disconnectCGM() {
    controlIQPtr = nullptr;
    cout << "CGM accidentaly disconnected from ControlIQ. Please reconnect for glucose monitoring" <<endl;
}