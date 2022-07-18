#ifndef _MONITOR_TASK_H_
#define _MONITOR_TASK_H_

#include <Scheduler.h>
#include <LeanTask.h>
#include <Task.h>
#include <evccDataClass.h>

#include <SPI.h>
#include <epd4in2b_V2.h>
#include <epdpaint.h>
#include <utility/images/ImageData.h>

#define USE_DEBUG 1
#if USE_DEBUG
#define Debug(__info) Serial.println(__info)
#else
#define Debug(__info)
#endif

#define MONITOR_UPDATEINTERVAL 10000 // 10sec
#define MONITOR_MAXINTERVAL 900000

#define MODE_WELCOME 0
#define MODE_MAIN 1
#define MODE_CLEAR 2

#define COLORED 0
#define UNCOLORED 1

#define RED 0
#define BLACK 1

#define FILLED 0
#define NONEFILLED 1

#define MODE_OFF "off"
#define MODE_PV "pv"
#define MODE_MINPV "minpv"
#define MODE_FAST "now"

// Language Specific
const char txtchargepower[] = "LEISTUNG";
const char txtchargedEnergy[] = "GELADEN";
const char txtchargeRemainingDuration[] = "RESTZEIT";
const char txtchargechargeDuration[] = "DAUER";
const char txtvehicleCapacity[] = "LADESTAND";
const char txttargetSoC[] = "LADEZIEL";
const char txtmode[] = "Modus";
const char txtmodeoff[] = "Aus";
const char txtmodepv[] = "PV";
const char txtmodeminpv[] = "Min+PV";
const char txtmodenow[] = "Schnell";

class MonitorTask : public LeanTask
{
public:
    void setEvccApiDataPtr(evccDataClass *ptr) { this->evccapidataptr = ptr; };
    void setWifiAvaliable();
    void setWifiDefaultUserPW(const char *WIFI_AP_SSID, const char *WIFI_AP_DEFAULT_PASSWORD, const char *ADMIN_PASSWORD);

protected:
    void setup();
    void loop();

    bool UpdateRequired = false;

private:
    // Display variables
    Epd epd;
    int percentToXposition(int percent);
    void WelcomeScreen(void);
    void MainScreen(void);
    void getTime(long long Duration, char *outStr);
    void getEvccApiData(void);
    void setDisplay();
    void DrawStringToDisplay(Paint *paint, int x, int y, const char *text, sFONT *font, int colored, int color);
    void DrawImageToDisplay(Paint *paint, int x, int y, const struct sIMG *image, int color);
    void DrawRectangleToDisplay(Paint *paint, int x, int y, int w, int h, int fill, int color);
    bool match(const char *ptr1, const char *ptr2);
    byte Mode = MODE_CLEAR;
    byte ModeOld = 255;
    unsigned long UpdatelastTime = 0;
    bool WifiAvaliable = false;
    char Wifi_ap_ssid[64] = "EVCC E-Ink Monitor";
    char Wifi_ap_password[64] = "evccMonitor";
    char Wifi_admin_password[64] = "";
    bool InitalStart = true;

    evccDataClass *evccapidataptr;
};

#endif