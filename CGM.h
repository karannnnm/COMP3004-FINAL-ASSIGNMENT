#ifndef CGM_H
#define CGM_H

#include <vector>
#include <ctime>
#include "ControlIQ.h"

class CGM {
public:
    CGM();
    
    // Link to ControlIQ's glucose level
    void linkToControlIQ(const ControlIQ& controlIQ);

    void disconnectCGM();
    
    // Update glucose reading (called every 15 seconds)
    void updateGlucoseReading();
    
    // Get latest glucose reading
    double getCurrentGlucose() const;
    
    // data for graph. 
    const std::vector<std::pair<time_t, double>>& getGlucoseHistory() const;

private:

    

    const ControlIQ* controlIQPtr;
    std::vector<std::pair<time_t, double>> glucoseHistory; // simple storage fpr time and glucose value pairs
};

#endif