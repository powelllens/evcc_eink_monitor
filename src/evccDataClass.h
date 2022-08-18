#ifndef _EVCC_DATA_CLASS_
#define _EVCC_DATA_CLASS_

#include <Arduino.h>

struct AvgData
{
    int avgraw;
    int sumraw;
    byte count;
    byte maxcount;
    double avg;
    double last;
};

struct LoadpointData
{
    int activePhases;                  // 1P|3P
    int targetSoC;                     // %
    int minSoC;                        //%
    int vehicleSoC;                    // %
    int vehicleRange;                  // km
    int chargePower;                   // Watts
    double chargedEnergy;              // Watts
    long long chargeRemainingDuration; // sec
    long long chargeDuration;          // sec
    bool charging;
    bool connected;
    char mode[30] = "";
    char title[30] = "";
    char vehicleTitle[30] = "";
};

struct SitePower
{
    float actual_gridPower;
    float actual_pvPower;
    AvgData avg_gridpower = {0, 0, 0, 240, 0.0, 0.0};
    AvgData avg_pvpower = {0, 0, 0, 240, 0.0, 0.0};
    int hist_gridPower[72];
    int hist_pvPower[72];
    int hist_max;
    int hist_min;
};

struct SiteData
{
    char siteTitle[30] = "";
    LoadpointData loadPointData[2];
    SitePower sitePower;
};

class evccDataClass
{
private:
    bool newdataavaliable;

public:
    SiteData globalapidata;
    void setNewData(bool newdata) { this->newdataavaliable = newdata; }
    void setClearNew() { this->newdataavaliable = false; }
    bool getNewData() const { return this->newdataavaliable; }
    SiteData getData() const { return this->globalapidata; }
    double getAvgData(AvgData *avgdata) { return avgdata->avg; };
    void addAvgData(AvgData *avgdata, int newdata);
    void setDataUpateInterval(int updateinterval)
    {
        this->globalapidata.sitePower.avg_gridpower.maxcount = 1200 / updateinterval;
        this->globalapidata.sitePower.avg_pvpower.maxcount = 1200 / updateinterval;
    };
};
#endif