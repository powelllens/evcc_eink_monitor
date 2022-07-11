#ifndef _EVCCREST_H_
#define _EVCCREST_H_

#include <Arduino.h>

#define EVCC_API_UPDATEINTERVAL 10000 // 10sec

struct EvccLoadpointData
{
    int activePhases;                     // 1P|3P
    int targetSoC;                        // %
    int minSoC;                           //%
    int vehicleSoC;                       // %
    int vehicleRange;                     // km
    int chargePower;                      // Watts
    double chargedEnergy;                 // Watts
    long long chargeRemainingDuration; // sec
    long long chargeDuration; // sec
    bool charging;
    bool connected;
    char mode[30] = "";
    char title[30] = "";
    char vehicleTitle[30] = "";
};

struct EvccAPIData
{
    char siteTitle[30] = "";
    EvccLoadpointData loadPointData[1];
};

class EVCCRestAPI
{
public:
    void updateData(void);
    EvccAPIData RestAPIData;
    void setServerConfig(const char *servername, unsigned int serverport);
    bool MonitorUpdateAvaliable = false;

private:
    unsigned long lastAPIUpdate;

    char servername[64] = "";
    unsigned int serverport;
    String httpGETRequest(const char *serverName);
    bool match(const char *ptr1, const char *ptr2);
};

#endif