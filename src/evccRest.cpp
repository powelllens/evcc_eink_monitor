#include <evccRest.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DEV_Config.h>
#include <utility/Debug.h>

void EVCCRestAPI::setServerConfig(const char *ServerName, unsigned int ServerPort)
{
    strcpy(this->servername, ServerName);
    this->serverport = ServerPort;
}

void EVCCRestAPI::updateData()
{
    if (DEV_Check_Time(this->lastAPIUpdate, EVCC_API_UPDATEINTERVAL))
    {
        Debug("Start Request...");
        char serverPath[160] = "";
        sprintf(serverPath, "http://%s:%i/api/state", this->servername, this->serverport);
        Debug(serverPath);
        String jsonBuffer;
        jsonBuffer = httpGETRequest(serverPath);

        DynamicJsonDocument doc(3500);

        DeserializationError error = deserializeJson(doc, jsonBuffer);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            this->lastAPIUpdate = millis();
            return;
        }
        JsonObject result = doc["result"];

        strcpy(this->RestAPIData.siteTitle, result["siteTitle"]);
        JsonObject result_loadpoints_0 = result["loadpoints"][0];
        EvccLoadpointData LoadpointData;

        LoadpointData = this->RestAPIData.loadPointData[0];

        int intbuffer;
        bool boolbuffer;

        bool tmp_updateavaliable = false;
        LoadpointData.activePhases = result_loadpoints_0["activePhases"];
        intbuffer = LoadpointData.targetSoC;
        LoadpointData.targetSoC = result_loadpoints_0["targetSoC"];
        tmp_updateavaliable = (tmp_updateavaliable) || (intbuffer != LoadpointData.targetSoC);
        LoadpointData.vehicleSoC = result_loadpoints_0["vehicleSoC"];
        LoadpointData.minSoC = result_loadpoints_0["minSoC"];
        LoadpointData.vehicleRange = result_loadpoints_0["vehicleRange"];
        LoadpointData.chargePower = result_loadpoints_0["chargePower"];
        LoadpointData.chargedEnergy = result_loadpoints_0["chargedEnergy"];
        LoadpointData.chargeRemainingDuration = result_loadpoints_0["chargeRemainingDuration"];
        LoadpointData.chargeDuration = result_loadpoints_0["chargeDuration"];
        boolbuffer = LoadpointData.charging;
        LoadpointData.charging = result_loadpoints_0["charging"];
        tmp_updateavaliable = tmp_updateavaliable || (boolbuffer != LoadpointData.charging);
        boolbuffer = LoadpointData.connected;
        LoadpointData.connected = result_loadpoints_0["connected"];
        tmp_updateavaliable = tmp_updateavaliable || (boolbuffer != LoadpointData.connected);
        tmp_updateavaliable = tmp_updateavaliable || (!match(LoadpointData.mode, result_loadpoints_0["mode"]));

        strcpy(LoadpointData.mode, result_loadpoints_0["mode"]);
        strcpy(LoadpointData.title, result_loadpoints_0["title"]);
        strcpy(LoadpointData.vehicleTitle, result_loadpoints_0["vehicleTitle"]);

        this->RestAPIData.loadPointData[0] = LoadpointData;
        this->MonitorUpdateAvaliable = tmp_updateavaliable;

        this->lastAPIUpdate = millis();
    }
}

// return true if two C strings are equal
bool EVCCRestAPI::match(const char *ptr1, const char *ptr2)
{
    // zero bytes at ends
    while (*ptr1 != 0 && *ptr2 != 0)
    {
        if (*ptr1 != *ptr2)
        {
            return false;
        }
        ++ptr1;
        ++ptr2;
    }
    return (*ptr1 == *ptr2);
}

String EVCCRestAPI::httpGETRequest(const char *serverName)
{
    WiFiClient client;
    HTTPClient http;

    // Your IP address with path or Domain name with URL path
    http.begin(client, serverName);

    // Send HTTP POST request
    int httpResponseCode = http.GET();

    String payload = "{}";

    if (httpResponseCode > 0)
    {
        // Serial.print("HTTP Response code: ");
        // Serial.println(httpResponseCode);
        payload = http.getString();
        // Serial.print(payload);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}
