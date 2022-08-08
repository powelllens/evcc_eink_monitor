#include "MonitorTask.h"
#include <evccDataClass.h>
#include <RestTask.h>

void MonitorTask::setup()
{
    Serial.println("Monitor Task Start");
}

void MonitorTask::loop()
{
    if (!this->WifiAvaliable && this->Mode == MODE_IDLE && this->InitalStart)
    {
        this->Mode = MODE_WELCOME;
        this->InitalStart = false;
    }

    this->setDisplay();
    this->getEvccApiData();

    this->delay(MONITOR_UPDATEINTERVAL);
}

void MonitorTask::setDisplay()
{
    if ((this->Mode != this->ModeOld || this->UpdateRequired))
    {
        this->UpdateRequired = false;
        this->UpdatelastTime = millis();
        this->ModeOld = this->Mode;
        Debug("Start Init mon");
        if (epd.Init() != 0)
        {
            Debug("e-Paper init failed");
            return;
        }
        switch (this->Mode)
        {
        case MODE_IDLE:
            Debug("Mode clear");
            this->IdleScreen();
            break;
        case MODE_WELCOME:
            Debug("Mode Welcome");
            this->WelcomeScreen();
            break;
        case MODE_MAIN:
            Debug("Mode Main");
            this->MainScreen();
            break;
        }
        this->epd.DisplayFrame();
        this->epd.Sleep();
    }
}

void MonitorTask::setIdleMode(char State[STRING_LEN])
{
    if (match(State, IDLE_MODE_CLEAR_txt))
    {
        this->IdleMode = IDLE_MODE_CLEAR;
    };
    if (match(State, IDLE_MODE_GRAPH_txt))
    {
        this->IdleMode = IDLE_MODE_GRAPH;
    };
    if (match(State, IDLE_MODE_LOGO_txt))
    {
        this->IdleMode = IDLE_MODE_LOGO;
    };
}

void MonitorTask::setWifiDefaultUserPW(const char *WIFI_AP_SSID, const char *WIFI_AP_DEFAULT_PASSWORD, const char *ADMIN_PASSWORD)
{
    strcpy(this->Wifi_ap_ssid, WIFI_AP_SSID);
    strcpy(this->Wifi_ap_password, WIFI_AP_DEFAULT_PASSWORD);
    strcpy(this->Wifi_admin_password, ADMIN_PASSWORD);
}

int MonitorTask::percentToXposition(int percent)
{
    return 3.06 * percent + 52;
}

void MonitorTask::getEvccApiData()
{
    if (this->evccapidataptr->getNewData())
    {
        if (this->evccapidataptr->globalapidata.loadPointData[0].connected)
        {
            this->Mode = MODE_MAIN;
            this->UpdateRequired = true;
            this->evccapidataptr->setClearNew();
        }
        else
        {
            this->Mode = MODE_IDLE;
            this->UpdateRequired = true;
            this->evccapidataptr->setClearNew();
        }
    }

    if (((millis() - this->UpdatelastTime) > MONITOR_MAXINTERVAL) && (this->Mode != MODE_IDLE))
    {
        this->UpdateRequired = true;
    }
}

void MonitorTask::setWifiAvaliable()
{
    this->WifiAvaliable = true;
    if (this->Mode == MODE_WELCOME)
    {
        this->Mode = MODE_IDLE;
    }
}

void MonitorTask::IdleScreen()
{
    switch (this->IdleMode)
    {
    case IDLE_MODE_CLEAR:
        this->epd.ClearFrame();
        break;
    case IDLE_MODE_GRAPH:
        
        //this->evccapidataptr->globalapidata.sitePower.hist_gridpower[]

        break;
    case IDLE_MODE_LOGO:
        //  Create a new image cache named IMAGE_BW and fill it with white
        unsigned char image[2550];
        Paint paint(image, 400, 50); // width should be the multiple of 8
        Debug("Start Logo");

        this->epd.ClearFrame();

        int randx;
        int randy;

        randx = random(10, 300);
        randy = random(10, 260);
        // evcc Logo
        this->DrawImageToDisplay(&paint, randx, randy, &Image_EVCC_B, BLACK);
        this->DrawImageToDisplay(&paint, randx + 51, randy, &Image_EVCC_R, RED);
        break;
    }
}

void MonitorTask::WelcomeScreen()
{
    char buffer_char[128 + sizeof(char)];
    // Create a new image cache named IMAGE_BW and fill it with white
    unsigned char image[2550];
    Paint paint(image, 400, 50); // width should be the multiple of 8
    Debug("Start Welcome");

    this->epd.ClearFrame();
    this->DrawStringToDisplay(&paint, 12, 10, "Welcome to", &Font24, COLORED, BLACK);
    this->DrawStringToDisplay(&paint, 12, 40, "EVCC E-Ink Monitor Screen", &Font20, COLORED, BLACK);
    this->DrawStringToDisplay(&paint, 12, 60, "Connect to Accesspoint:", &Font20, COLORED, BLACK);
    this->DrawStringToDisplay(&paint, 12, 100, this->Wifi_ap_ssid, &Font20, UNCOLORED, RED);

    sprintf(buffer_char, "Key:   '%s'", this->Wifi_ap_password);
    this->DrawStringToDisplay(&paint, 12, 130, buffer_char, &Font20, COLORED, RED);

    this->DrawStringToDisplay(&paint, 12, 180, "User: 'admin'", &Font20, COLORED, BLACK);
    sprintf(buffer_char, "PW:   '%s'", this->Wifi_admin_password);
    this->DrawStringToDisplay(&paint, 12, 200, buffer_char, &Font20, COLORED, RED);

    // little Car Logo
    this->DrawImageToDisplay(&paint, 350, 260, &Image_Car, BLACK);

    // evcc Logo
    this->DrawImageToDisplay(&paint, 12, 260, &Image_EVCC_B, BLACK);
    this->DrawImageToDisplay(&paint, 63, 260, &Image_EVCC_R, RED);
}

void MonitorTask::MainScreen()
{
    char buffer_char[128 + sizeof(char)];
    // Create a new image cache named IMAGE_BW and fill it with white
    unsigned char image[2550];
    Paint paint(image, 400, 50); // width should be the multiple of 8
    Debug("Start Main");

    this->epd.ClearFrame();

    // siteTitle
    this->DrawStringToDisplay(&paint, 12, 12, this->evccapidataptr->globalapidata.siteTitle, &Font24, COLORED, BLACK);
    // title
    this->DrawStringToDisplay(&paint, 65, 58, this->evccapidataptr->globalapidata.loadPointData[0].title, &Font20, COLORED, BLACK);
    // vehicleTitle
    this->DrawStringToDisplay(&paint, 65, 85, this->evccapidataptr->globalapidata.loadPointData[0].vehicleTitle, &Font20, COLORED, BLACK);

    // Leistung
    this->DrawStringToDisplay(&paint, 12, 120, txtchargepower, &Font16, COLORED, BLACK);
    sprintf(buffer_char, "%.2f kW", (this->evccapidataptr->globalapidata.loadPointData[0].chargePower / 1000.0));
    this->DrawStringToDisplay(&paint, 12, 134, buffer_char, &Font16, COLORED, BLACK);

    // Geladen
    this->DrawStringToDisplay(&paint, 165, 120, txtchargedEnergy, &Font16, COLORED, BLACK);
    sprintf(buffer_char, "%.2f kWh", (this->evccapidataptr->globalapidata.loadPointData[0].chargedEnergy / 1000.0));
    this->DrawStringToDisplay(&paint, 165, 134, buffer_char, &Font16, COLORED, BLACK);

    // Restzeit
    if (this->evccapidataptr->globalapidata.loadPointData[0].chargeRemainingDuration == -1)
    {
        this->DrawStringToDisplay(&paint, 315, 120, txtchargechargeDuration, &Font16, COLORED, BLACK);
        this->getTime(this->evccapidataptr->globalapidata.loadPointData[0].chargeDuration, buffer_char);
        this->DrawStringToDisplay(&paint, 315, 134, buffer_char, &Font16, COLORED, BLACK);
    }
    else
    {
        this->DrawStringToDisplay(&paint, 315, 120, txtchargeRemainingDuration, &Font16, COLORED, BLACK);
        this->getTime(this->evccapidataptr->globalapidata.loadPointData[0].chargeRemainingDuration, buffer_char);
        this->DrawStringToDisplay(&paint, 315, 134, buffer_char, &Font16, COLORED, BLACK);
    }

    // little Car Logo
    this->DrawImageToDisplay(&paint, 12, 72, &Image_Car, BLACK);
    // end of top half content

    paint.SetWidth(400);
    paint.SetHeight(50);
    paint.Clear(UNCOLORED);

    paint.DrawRectangle(50, 5, 360, 41, COLORED);
    // Rectangle for Loadingstatus and targetSoc (in black if vehicleCapacity >= vehicleSoC )
    if (this->evccapidataptr->globalapidata.loadPointData[0].vehicleSoC >= this->evccapidataptr->globalapidata.loadPointData[0].minSoC)
    {
        paint.DrawFilledRectangle(52, 7, percentToXposition(this->evccapidataptr->globalapidata.loadPointData[0].vehicleSoC), 39, COLORED);
    }
    int positionX = percentToXposition(this->evccapidataptr->globalapidata.loadPointData[0].targetSoC);
    paint.DrawFilledRectangle(positionX - 4, 0, positionX + 4, 48, COLORED);
    this->epd.SetPartialWindowBlack(paint.GetImage(), 0, 163, 400, 50);

    // Ladestand
    this->DrawStringToDisplay(&paint, 12, 215, txtvehicleCapacity, &Font16, COLORED, BLACK);
    sprintf(buffer_char, "%d %s", this->evccapidataptr->globalapidata.loadPointData[0].vehicleSoC, "%");
    this->DrawStringToDisplay(&paint, 12, 230, buffer_char, &Font16, COLORED, BLACK);
    sprintf(buffer_char, "%d %s", this->evccapidataptr->globalapidata.loadPointData[0].vehicleRange, "km");
    this->DrawStringToDisplay(&paint, 12, 245, buffer_char, &Font16, COLORED, BLACK);
    // Ladeziel
    this->DrawStringToDisplay(&paint, 310, 215, txttargetSoC, &Font16, COLORED, BLACK);
    sprintf(buffer_char, "%d %s", this->evccapidataptr->globalapidata.loadPointData[0].targetSoC, "%");
    this->DrawStringToDisplay(&paint, 310, 230, buffer_char, &Font16, COLORED, BLACK);

    // Modus
    if (match(this->evccapidataptr->globalapidata.loadPointData[0].mode, MODE_OFF))
    {
        // OFF
        sprintf(buffer_char, "%s: %s", txtmode, txtmodeoff);
    }
    else if (match(this->evccapidataptr->globalapidata.loadPointData[0].mode, MODE_PV))
    {
        // PV
        sprintf(buffer_char, "%s: %s", txtmode, txtmodepv);
    }
    else if (match(this->evccapidataptr->globalapidata.loadPointData[0].mode, MODE_MINPV))
    {
        // MINPV
        sprintf(buffer_char, "%s: %s", txtmode, txtmodeminpv);
    }
    else
    {
        // NOW
        sprintf(buffer_char, "%s: %s", txtmode, txtmodenow);
    }
    this->DrawStringToDisplay(&paint, 190, 270, buffer_char, &Font20, COLORED, BLACK);

    // Strike in black - not loading
    if (not this->evccapidataptr->globalapidata.loadPointData[0].charging)
    {
        this->DrawImageToDisplay(&paint, 12, 165, &Image_Strike, BLACK);
    }

    // Ladestand - in red due to vehicleCapacity < vehicleSoC
    if (this->evccapidataptr->globalapidata.loadPointData[0].vehicleSoC < this->evccapidataptr->globalapidata.loadPointData[0].minSoC)
    {
        paint.SetWidth(400);
        paint.SetHeight(50);
        paint.Clear(UNCOLORED);
        paint.DrawFilledRectangle(52, 7, percentToXposition(this->evccapidataptr->globalapidata.loadPointData[0].vehicleSoC), 39, COLORED);
        this->epd.SetPartialWindowBlack(paint.GetImage(), 0, 163, 400, 50);
    }

    // Strike in red while loading
    if (this->evccapidataptr->globalapidata.loadPointData[0].charging)
    {
        this->DrawImageToDisplay(&paint, 12, 165, &Image_Strike, RED);
    }

    // evcc Logo
    this->DrawImageToDisplay(&paint, 12, 260, &Image_EVCC_B, BLACK);
    this->DrawImageToDisplay(&paint, 63, 260, &Image_EVCC_R, RED);
}

void MonitorTask::getTime(long long Duration, char *outStr)
{
    long long chargeRemainingDuration_hours = Duration / 3600000000000;
    long long chargeRemainingDuration_min = Duration / 60000000000 - chargeRemainingDuration_hours * 60;

    sprintf(outStr, "%lld:%lld h", chargeRemainingDuration_hours, chargeRemainingDuration_min);
}

void MonitorTask::DrawStringToDisplay(Paint *paint, int x, int y, const char *text, sFONT *font, int colored, int color)
{
    size_t Size = strlen(text);
    byte t_width = font->Width;
    byte t_height = font->Height;

    paint->SetHeight(t_height);
    paint->SetWidth(t_width * Size);

    if (x + paint->GetWidth() > 400)
    {
        Debug(F("Printing Text outside Window"));
    }

    paint->Clear(UNCOLORED);
    paint->DrawStringAt(0, 0, text, font, COLORED);
    if (color == BLACK)
    {
        this->epd.SetPartialWindowBlack(paint->GetImage(), x, y, paint->GetWidth(), paint->GetHeight());
    }
    else
    {
        this->epd.SetPartialWindowRed(paint->GetImage(), x, y, paint->GetWidth(), paint->GetHeight());
    }
}

void MonitorTask::DrawImageToDisplay(Paint *paint, int x, int y, const struct sIMG *image, int color)
{
    paint->SetHeight(image->Height);
    paint->SetWidth(image->Width);

    if (x + paint->GetWidth() > 400)
    {
        Debug(F("Printing Image outside Window"));
    }

    paint->Clear(UNCOLORED);
    paint->DrawImage(image->Image, 0, 0, image->Width, image->Height);

    if (color == BLACK)
    {
        this->epd.SetPartialWindowBlack(paint->GetImage(), x, y, paint->GetWidth(), paint->GetHeight());
    }
    else
    {
        this->epd.SetPartialWindowRed(paint->GetImage(), x, y, paint->GetWidth(), paint->GetHeight());
    }
}

// return true if two C strings are equal
bool MonitorTask::match(const char *ptr1, const char *ptr2)
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
