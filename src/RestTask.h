#ifndef _REST_TASK_H_
#define _REST_TASK_H_

#include <Scheduler.h>
#include <LeanTask.h>

#include <evccDataClass.h>

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define REST_UPDATEINTERVAL 10000 // 10sec

class RestTask : public LeanTask
{
public:
    evccDataClass evccapidata;
    evccDataClass *getAPIData() { return &this->evccapidata; };
    void setServerConfig(const char *servername, unsigned int serverport);
    void setWifiAvaliable() { this->WifiAvaliable = true; };

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
};

#endif