#ifndef _EVCCMON_H_
#define _EVCCMON_H_

#include <Arduino.h>
#include <EPD.h>
#include <evccRest.h>

#define DISPLAYSTATE_BOOT 0
#define DISPLAYSTATE_CLEAR 1
#define DISPLAYSTATE_WELLCOME 2
#define DISPLAYSTATE_MAIN 3
#define DISPLAYSTATE_STARTSLEEP 4
#define DISPLAYSTATE_SLEEP 5

// Language Specific
const char txtchargepower[] = "LEISTUNG";
const char txtchargedEnergy[] = "GELADEN";
const char txtchargeRemainingDuration[] = "RESTZEIT";
const char txtchargechargeDuration[] = "DAUER";
const char txtvehicleCapacity[] = "LADESTAND";
const char txttargetSoC[] = "LADEZIEL";
const char txtmode[] = "Modus";

class EVCCMonitor
{
public:
    void InitMonitor();
    void doLoop();
    EvccAPIData evccAPIData;
    bool UpdateRequired = false;
private:
    // Display variables
    EPD_4IN2B_V2 epd_4in2b_v2;
    int percentToXposition(int percent);
    void WelcomeScreen(void);
    void MainScreen(void);
    bool DisplayActive = false;
    byte DisplayState = DISPLAYSTATE_BOOT;
    unsigned long DisplayUpdatelastTime = 0;
    unsigned long DisplayUpdatetimerDelay = 900000;
    void getTime(long long Duration,char* outStr);
};

#endif