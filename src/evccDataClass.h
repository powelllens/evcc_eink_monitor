#ifndef _EVCC_DATA_CLASS_
#define _EVCC_DATA_CLASS_

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

struct SiteData
{
    char siteTitle[30] = "";
    LoadpointData loadPointData[2];
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
};
#endif