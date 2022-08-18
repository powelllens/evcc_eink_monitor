#ifndef _REST_TASK_H_
#define _REST_TASK_H_

#include <Scheduler.h>
#include <LeanTask.h>

#include <evccDataClass.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

/* Configuration of NTP */
#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"

class RestTask : public LeanTask
{
public:
    evccDataClass evccapidata;
    evccDataClass *getAPIData() { return &this->evccapidata; };
    void setServerConfig(const char *servername, unsigned int serverport);
    void setWifiAvaliable() { this->WifiAvaliable = true; };
    void setUpdateInterval(u16 interval)
    {
        this->REST_UPDATEINTERVAL = interval * 1000;
        this->evccapidata.setDataUpateInterval(interval);
    };

protected:
    void setup();
    void loop();

private:
    void updateData(void);
    bool match(const char *ptr1, const char *ptr2);
    String httpGETRequest(const char *serverName);
    unsigned long lastAPIUpdate;
    char servername[64] = "";
    unsigned int serverport;

    bool ServerConfigured = false;
    bool WifiAvaliable = false;

    u32 REST_UPDATEINTERVAL = 10000;
    time_t now;   // this is the epoch
    tm tm_struct; // the structure tm holds time information in a more convient way
    bool graphtimetriggered = false;
};

#endif