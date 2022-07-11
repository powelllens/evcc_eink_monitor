#include <Arduino.h>
#include <IotWebConf.h>
#include <IotWebConfTParameter.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DEV_Config.h>
#include <config.h>
#include <evccRest.h>
#include <evccMonitor.h>

// UpdateServer includes
#ifdef ESP8266
#include <ESP8266HTTPUpdateServer.h>
#elif defined(ESP32)
// For ESP32 IotWebConf provides a drop-in replacement for UpdateServer.
#include <IotWebConfESP32HTTPUpdateServer.h>
#endif

// -- Callback method declarations.
void handleRoot();
void configSaved();
void wifiConnected();
bool formValidator(iotwebconf::WebRequestWrapper *webRequestWrapper);

DNSServer dnsServer;
WebServer server(80);
#ifdef ESP8266
ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ESP32)
HTTPUpdateServer httpUpdater;
#endif

#define STRING_LEN 64
#define NUMBER_LEN 32

IotWebConf iotWebConf(WIFI_AP_SSID, &dnsServer, &server, WIFI_AP_DEFAULT_PASSWORD, CONFIG_VERSION);

iotwebconf::ParameterGroup evccsettings = iotwebconf::ParameterGroup("evccsettings", "EVCC Settings");
iotwebconf::TextTParameter<STRING_LEN> servername =
    iotwebconf::Builder<iotwebconf::TextTParameter<STRING_LEN>>("servername").label("Server Name").defaultValue("evcc.local").build();
iotwebconf::IntTParameter<u32> serverport =
    iotwebconf::Builder<iotwebconf::IntTParameter<u32>>("serverport").label("Server Port").defaultValue(7070).min(1).max(99999).step(1).placeholder("1..99999").build();

boolean needReset = false;
boolean connected = false;

// Rest API variables
EVCCRestAPI evccrestapi;
void getEvccApiData(void);

// Monitor variables
#define DisplayUpdatetimerDelay 900000
unsigned long DisplayUpdatelastTime = 0;
EVCCMonitor evccmonitor;

void setup()
{
  DEV_Module_Init();

  Serial.println();
  Serial.println("Starting up...");

  evccsettings.addItem(&servername);
  evccsettings.addItem(&serverport);

  iotWebConf.addParameterGroup(&evccsettings);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.setFormValidator(&formValidator);
  iotWebConf.getApTimeoutParameter()->visible = true;

  // -- Define how to handle updateServer calls.
  iotWebConf.setupUpdateServer(
      [](const char *updatePath)
      { httpUpdater.setup(&server, updatePath); },
      [](const char *userName, char *password)
      { httpUpdater.updateCredentials(userName, password); });

  // -- Initializing the configuration.
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []
            { iotWebConf.handleConfig(); });
  server.onNotFound([]()
                    { iotWebConf.handleNotFound(); });

  evccrestapi.setServerConfig(servername.value(), serverport.value());

  evccmonitor.InitMonitor();

  Serial.println("Ready.");
}

void loop()
{
  // -- doLoop should be called as frequently as possible.
  if (needReset)
  {
    // Doing a chip reset caused by config changes
    printf("Rebooting after 1 second.");
    delay(1000);
    ESP.restart();
  }

  getEvccApiData();
  evccmonitor.doLoop();
  iotWebConf.doLoop();
  yield();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>EVCC E-Ink Monitor</title></head><body><div style=\"text-align:left;display:inline-block;min-width:260px;\">Status page of ";
  s += iotWebConf.getThingName();
  s += ".</div>";
  s += "<ul>";
  s += "<li>EVCC Server Name: ";
  s += servername.value();
  s += "<li>EVCC Server Port: ";
  s += serverport.value();
  s += "<li>EVCC Title: ";
  s += evccmonitor.evccAPIData.siteTitle;
  s += "<li>EVCC Loadpoint Title: ";
  s += evccmonitor.evccAPIData.loadPointData[0].title;
  s += "<li>Car Title: ";
  s += evccmonitor.evccAPIData.loadPointData[0].vehicleTitle;
  s += "<li>Car Connected: ";
  s += evccmonitor.evccAPIData.loadPointData[0].connected;
  s += "<li>Car charging: ";
  s += evccmonitor.evccAPIData.loadPointData[0].charging;
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void configSaved()
{
  Serial.println("Configuration was updated.");
  needReset = true;
}

void wifiConnected()
{
  Serial.println("WiFi connection established.");
  connected = true;
}

bool formValidator(iotwebconf::WebRequestWrapper *webRequestWrapper)
{
  Serial.println("Validating form.");
  bool valid = true;

  /*
    int l = webRequestWrapper->arg(stringParam.getId()).length();
    if (l < 3)
    {
      stringParam.errorMessage = "Please provide at least 3 characters for this test!";
      valid = false;
    }
  */
  return valid;
}

void getEvccApiData()
{
  // Send an HTTP GET request
  if (connected)
  {
    evccrestapi.updateData();
    if (evccrestapi.MonitorUpdateAvaliable || (evccrestapi.RestAPIData.loadPointData[0].connected && DEV_Check_Time(DisplayUpdatelastTime,DisplayUpdatetimerDelay)))
    {
      evccmonitor.UpdateRequired = true;
      evccmonitor.evccAPIData = evccrestapi.RestAPIData;
      evccrestapi.MonitorUpdateAvaliable = false;
      DisplayUpdatelastTime = millis();
    }
  }
}