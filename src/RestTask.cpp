#include "RestTask.h"

void RestTask::setup()
{
    Serial.println("Rest Task Start");
}

void RestTask::loop()
{
    if (this->ServerConfigured && this->WifiAvaliable)
    {
        this->updateData();
    }
    this->delay(REST_UPDATEINTERVAL);
}

void RestTask::setServerConfig(const char *ServerName, unsigned int ServerPort)
{
    strcpy(this->servername, ServerName);
    this->serverport = ServerPort;
    this->ServerConfigured = true;
}

void RestTask::updateData()
{
    char serverPath[160] = "";
    sprintf(serverPath, "http://%s:%i/api/state", this->servername, this->serverport);
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
    this->evccapidata.globalapidata.sitePower.actual_gridPower = result["gridPower"];
    this->evccapidata.globalapidata.sitePower.actual_pvPower = result["pvPower"];
    strcpy(this->evccapidata.globalapidata.siteTitle, result["siteTitle"] | "N/A");
    JsonObject result_loadpoints_0 = result["loadpoints"][0];
    LoadpointData loadpointdata;

    loadpointdata = this->evccapidata.globalapidata.loadPointData[0];

    int intbuffer;
    bool boolbuffer;

    bool tmp_updateavaliable = false;
    loadpointdata.activePhases = result_loadpoints_0["activePhases"];
    intbuffer = loadpointdata.targetSoC;
    loadpointdata.targetSoC = result_loadpoints_0["targetSoC"];
    tmp_updateavaliable = (tmp_updateavaliable) || (intbuffer != loadpointdata.targetSoC);
    loadpointdata.vehicleSoC = result_loadpoints_0["vehicleSoC"];
    loadpointdata.minSoC = result_loadpoints_0["minSoC"];
    loadpointdata.vehicleRange = result_loadpoints_0["vehicleRange"];
    loadpointdata.chargePower = result_loadpoints_0["chargePower"];
    loadpointdata.chargedEnergy = result_loadpoints_0["chargedEnergy"];
    loadpointdata.chargeRemainingDuration = result_loadpoints_0["chargeRemainingDuration"];
    loadpointdata.chargeDuration = result_loadpoints_0["chargeDuration"];
    boolbuffer = loadpointdata.charging;
    loadpointdata.charging = result_loadpoints_0["charging"];
    tmp_updateavaliable = tmp_updateavaliable || (boolbuffer != loadpointdata.charging);
    boolbuffer = loadpointdata.connected;
    loadpointdata.connected = result_loadpoints_0["connected"];
    tmp_updateavaliable = tmp_updateavaliable || (boolbuffer != loadpointdata.connected);
    tmp_updateavaliable = tmp_updateavaliable || (!match(loadpointdata.mode, result_loadpoints_0["mode"]));

    strcpy(loadpointdata.mode, result_loadpoints_0["mode"] | "N/A");
    strcpy(loadpointdata.title, result_loadpoints_0["title"] | "N/A");
    strcpy(loadpointdata.vehicleTitle, result_loadpoints_0["vehicleTitle"] | "N/A");

    this->evccapidata.globalapidata.loadPointData[0] = loadpointdata;
    this->evccapidata.setNewData(tmp_updateavaliable);

    this->lastAPIUpdate = millis();
}

// return true if two C strings are equal
bool RestTask::match(const char *ptr1, const char *ptr2)
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

String RestTask::httpGETRequest(const char *serverName)
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
