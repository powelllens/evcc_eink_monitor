#ifndef _CONFIG_H
#define _CONFIG_H

#include "Arduino.h"

#define Language_German
#ifndef Language_German
#define Language_English
#endif

#define VERSION "1.0.0"

// Modifying the config version will probably cause a loss of the existig configuration.
// Be careful!
#define CONFIG_VERSION "1.0.0"

const char WIFI_AP_SSID[] = "EVCC E-Ink Monitor";
const char WIFI_AP_DEFAULT_PASSWORD[] = "evccMonitor";

#endif