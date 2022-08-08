#include <Arduino.h>
#include <Scheduler.h>
#include <Task.h>
#include <LeanTask.h>
#include <RestTask.h>
#include <MonitorTask.h>
#include <IotWebConf.h>
#include <IotWebConfUsing.h>
#include <IotWebConfTParameter.h>
#include <ESP8266HTTPClient.h>
#include <config.h>

// UpdateServer includes
#ifdef ESP8266
#include <ESP8266HTTPUpdateServer.h>
#elif defined(ESP32)
// For ESP32 IotWebConf provides a drop-in replacement for UpdateServer.
#include <IotWebConfESP32HTTPUpdateServer.h>
#endif

#define STRING_LEN 64
#define NUMBER_LEN 32

// -- Callback method declarations.
void handleRoot();
void configSaved();
void wifiConnected();
bool formValidator(iotwebconf::WebRequestWrapper *webRequestWrapper);
void setEvccApiDataPtr(evccDataClass *ptr);

DNSServer dnsServer;
WebServer server(80);
#ifdef ESP8266
ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ESP32)
HTTPUpdateServer httpUpdater;
#endif

char idleDisplayValue[STRING_LEN];

static char idleDisplayStates[][STRING_LEN] = {"clear", "graph", "logo"};
static char idleDisplayStateNames[][STRING_LEN] = {"Clear", "24h PV Graph", "EVCC Logo"};

IotWebConf iotWebConf(WIFI_AP_SSID, &dnsServer, &server, WIFI_AP_DEFAULT_PASSWORD, CONFIG_VERSION);

iotwebconf::ParameterGroup evccsettings = iotwebconf::ParameterGroup("evccsettings", "EVCC Settings");
iotwebconf::TextTParameter<STRING_LEN> servername = iotwebconf::Builder<iotwebconf::TextTParameter<STRING_LEN>>("servername").label("Server Name").defaultValue("evcc.local").build();
iotwebconf::IntTParameter<u32> serverport = iotwebconf::Builder<iotwebconf::IntTParameter<u32>>("serverport").label("Server Port").defaultValue(7070).min(1).max(99999).step(1).placeholder("1..99999").build();

iotwebconf::ParameterGroup displaysettings = iotwebconf::ParameterGroup("displaysettings", "Display Settings");
iotwebconf::IntTParameter<u16> dataupdatetime = iotwebconf::Builder<iotwebconf::IntTParameter<u16>>("dataupdatetime").label("Data Update Interval (s)").defaultValue(10).min(5).max(65000).step(1).placeholder("5..65000").build();
iotwebconf::IntTParameter<u16> displayupdatetime = iotwebconf::Builder<iotwebconf::IntTParameter<u16>>("displayupdatetime").label("Display Update Interval (s)").defaultValue(900).min(300).max(65000).step(1).placeholder("300..65000").build();
IotWebConfSelectParameter idleDisplayParam = IotWebConfSelectParameter("Select Display Idle", "chooseParam", idleDisplayValue, STRING_LEN, (char *)idleDisplayStates, (char *)idleDisplayStateNames, sizeof(idleDisplayStates) / STRING_LEN, STRING_LEN, idleDisplayStates[0]);

boolean needReset = false;

evccDataClass *evccapidataptr;

static RestTask restTask;
static MonitorTask monitorTask;

class IoTWebConfTask : public LeanTask
{
public:
  void setup()
  {
    Serial.println("Start IoT Task");
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
    iotWebConf.doLoop();
    this->delay(100);
  }
} iotwebconfTask;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("IoTWebConf Task Start");
  Serial.println("Starting up...");

  evccsettings.addItem(&servername);
  evccsettings.addItem(&serverport);

  displaysettings.addItem(&dataupdatetime);
  displaysettings.addItem(&displayupdatetime);
  displaysettings.addItem(&idleDisplayParam);

  iotWebConf.addParameterGroup(&evccsettings);
  iotWebConf.addParameterGroup(&displaysettings);
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

  Serial.println("Ready.");

  // Setup other Tasks
  // Set evccapidata in monitor class
  monitorTask.setEvccApiDataPtr(restTask.getAPIData());
  monitorTask.setWifiDefaultUserPW(WIFI_AP_SSID, WIFI_AP_DEFAULT_PASSWORD, (iotWebConf.getApPasswordParameter())->valueBuffer);
  evccapidataptr = restTask.getAPIData();

  restTask.setServerConfig(servername.value(), serverport.value());

  iotwebconfTask.setInterval(100);
  Scheduler.start(&iotwebconfTask);
  restTask.setUpdateInterval(dataupdatetime.value());
  Scheduler.start(&restTask);
  monitorTask.setUpdateInterval(displayupdatetime.value());
  monitorTask.setIdleMode(idleDisplayValue);
  Scheduler.start(&monitorTask);

  Scheduler.begin();
}

void loop() {}

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
  s += evccapidataptr->globalapidata.siteTitle;
  s += "<li>EVCC Loadpoint Title: ";
  s += evccapidataptr->globalapidata.loadPointData[0].title;
  s += "<li>Car Title: ";
  s += evccapidataptr->globalapidata.loadPointData[0].vehicleTitle;
  s += "<li>Car Connected: ";
  s += evccapidataptr->globalapidata.loadPointData[0].connected;
  s += "<li>Car charging: ";
  s += evccapidataptr->globalapidata.loadPointData[0].charging;
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
  restTask.setWifiAvaliable();
  monitorTask.setWifiAvaliable();
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