#ifndef _MONITOR_TASK_H_
#define _MONITOR_TASK_H_

#include <Arduino.h>
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

#define MONITOR_UPDATEINTERVAL 5000 // 5sec

#define MODE_WELCOME 0
#define MODE_MAIN 1
#define MODE_IDLE 2

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

#define IDLE_MODE_CLEAR_txt "clear"
#define IDLE_MODE_GRAPH_txt "graph"
#define IDLE_MODE_LOGO_txt "logo"

#define IDLE_MODE_CLEAR 0
#define IDLE_MODE_GRAPH 1
#define IDLE_MODE_LOGO 2

#define STRING_LEN 64

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
    void setUpdateInterval(u16 interval) { this->MONITOR_MAXINTERVAL = interval * 1000; };
    void setIdleMode(char State[STRING_LEN]);

protected:
    void setup();
    void loop();

    bool UpdateRequired = false;

private:
    // Display variables
    Epd epd;
    int percentToXposition(int percent);
    void IdleScreen(void);
    void WelcomeScreen(void);
    void MainScreen(void);
    void getTime(long long Duration, char *outStr);
    void getEvccApiData(void);
    void setDisplay();
    void DrawStringToDisplay(Paint *paint, int x, int y, const char *text, sFONT *font, int colored, int color);
    void DrawImageToDisplay(Paint *paint, int x, int y, const struct sIMG *image, int color);
    void DrawRectangleToDisplay(Paint *paint, int x, int y, int w, int h, int fill, int color);
    bool match(const char *ptr1, const char *ptr2);
    int getYfromX(double m, double b, int x);
    byte Mode = MODE_IDLE;
    byte ModeOld = 255;
    unsigned long UpdatelastTime = 0;
    bool WifiAvaliable = false;
    char Wifi_ap_ssid[STRING_LEN] = "EVCC E-Ink Monitor";
    char Wifi_ap_password[STRING_LEN] = "evccMonitor";
    char Wifi_admin_password[STRING_LEN] = "";
    bool InitalStart = true;
    u32 MONITOR_MAXINTERVAL = 900000;
    byte IdleMode = IDLE_MODE_CLEAR;

    evccDataClass *evccapidataptr;
};

#endif