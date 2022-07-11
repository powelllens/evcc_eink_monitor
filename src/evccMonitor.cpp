#include <evccMonitor.h>
#include <utility/GUI_Paint.h>
#include <utility/images/ImageData.h>
#include <evccRest.h>
#include <config.h>

void EVCCMonitor::doLoop()
{
    if (this->epd_4in2b_v2.getInitState() && this->epd_4in2b_v2.getReady())
    {
        switch (this->DisplayState)
        {
        case DISPLAYSTATE_BOOT:
            Debug("cs boot");
            // this->epd_4in2b_v2.Clear();
            this->DisplayState = DISPLAYSTATE_CLEAR;
            break;
        case DISPLAYSTATE_CLEAR:
            Debug("cs clear");
            this->WelcomeScreen();
            this->epd_4in2b_v2.TurnOnDisplay();
            this->DisplayState = DISPLAYSTATE_WELLCOME;
            break;
        case DISPLAYSTATE_WELLCOME:

            if (this->UpdateRequired)
            {
                Debug("cs welcome");
                this->DisplayState = DISPLAYSTATE_MAIN;
            }
            break;
        case DISPLAYSTATE_MAIN:

            if (this->UpdateRequired)
            {
                Debug("cs Main");
                this->UpdateRequired = false;
                if (this->evccAPIData.loadPointData[0].connected)
                {
                    this->MainScreen();
                    epd_4in2b_v2.TurnOnDisplay();
                }
                else
                {
                    epd_4in2b_v2.Clear();
                    DisplayState = DISPLAYSTATE_STARTSLEEP;
                }
            }
            break;
        case DISPLAYSTATE_STARTSLEEP:
            Debug("cs start sleep");
            //this->epd_4in2b_v2.Sleep();
            this->DisplayState = DISPLAYSTATE_SLEEP;
            break;
        case DISPLAYSTATE_SLEEP:

            if (this->UpdateRequired)
            {
                Debug("cs Sleep");
                //this->epd_4in2b_v2.InitState = 0;
                //this->epd_4in2b_v2.Init();
                this->DisplayState = DISPLAYSTATE_MAIN;
            }
            break;
        }
    }
    this->epd_4in2b_v2.doLoop();
}

void EVCCMonitor::InitMonitor()
{
    this->epd_4in2b_v2.Init();
}

int EVCCMonitor::percentToXposition(int percent)
{
    return 3.06 * percent + 52;
}

void EVCCMonitor::WelcomeScreen()
{
    // Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *Image; // black \Red or Yellow
    UWORD Imagesize = ((EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1)) * EPD_4IN2B_V2_HEIGHT;
    if ((Image = (UBYTE *)malloc(Imagesize / 2)) == NULL)
    {
        printf("Failed to apply for half memory...");
        while (1)
            ;
    }
    // printf("NewImage:BlackImage and RYImage");
    Paint_NewImage(Image, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT / 2, 0, WHITE);

    // Select Image
    Paint_SelectImage(Image);
    char buffer_char[40 + sizeof(char)];

    // 1.Draw black image top half
    Paint_Clear(WHITE);
    // top half content here...

    Paint_DrawString_EN(12, 20, "Welcome to", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(12, 50, "EVCC E-Ink Monitor Screen", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(20, 85, "Connect to Accesspoint:", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(20, 105, WIFI_AP_SSID, &Font20, WHITE, BLACK);
    Paint_DrawString_EN(20, 130, "User: 'admin'",&Font20, WHITE, BLACK);

    // little Car Logo
    Paint_DrawImage(image_car, 348, 12, 40, 28);
    // end of top half content

    epd_4in2b_v2.SendHalfBimage(0, Image);

    // 2.Draw black image bottom half
    Paint_Clear(WHITE);
    // bottom half content here...

    sprintf(buffer_char, "PW:   '%s'", WIFI_AP_DEFAULT_PASSWORD);
    Paint_DrawString_EN(20, 0, buffer_char, &Font20, WHITE, BLACK);

    // First half of EVCC Logo
    Paint_DrawImage(image_evcc_1, 12, 110, 121, 34);

    // end of bottom half content
    epd_4in2b_v2.SendHalfBimage(1, Image);

    // 3.Draw red image top half
    Paint_Clear(WHITE);
    // top half content here...

    // end of top half content
    epd_4in2b_v2.SendHalfRYimage(0, Image);

    // 4.Draw red image bottom half
    Paint_Clear(WHITE);
    // bottom half content here...

    // rest of EVCC Logo
    Paint_DrawImage(image_evcc_2, 63, 110, 17, 34);

    // end of bottom half content
    epd_4in2b_v2.SendHalfRYimage(1, Image);

    free(Image);
    Image = NULL;
}

void EVCCMonitor::MainScreen()
{
    // This function will update the Main Screen with all global variables and send it to the Screen.

    // Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *Image; // black \Red or Yellow
    UWORD Imagesize = ((EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1)) * EPD_4IN2B_V2_HEIGHT;
    if ((Image = (UBYTE *)malloc(Imagesize / 2)) == NULL)
    {
        printf("Failed to apply for half memory...");
        while (1)
            ;
    }
    // printf("NewImage:BlackImage and RYImage");
    Paint_NewImage(Image, EPD_4IN2B_V2_WIDTH, EPD_4IN2B_V2_HEIGHT / 2, 0, WHITE);

    // Select Image
    Paint_SelectImage(Image);
    char buffer_char[40 + sizeof(char)];

    // 1.Draw black image top half
    Paint_Clear(WHITE);
    // top half content here...

    // siteTitle
    Paint_DrawString_EN(12, 12, this->evccAPIData.siteTitle, &Font24, WHITE, BLACK);
    // title
    Paint_DrawString_EN(65, 58, this->evccAPIData.loadPointData[0].title, &Font20, WHITE, BLACK);
    // vehicleTitle
    Paint_DrawString_EN(65, 85, this->evccAPIData.loadPointData[0].vehicleTitle, &Font20, WHITE, BLACK);

    // Leistung
    Paint_DrawString_EN(12, 120, txtchargepower, &Font12, WHITE, BLACK);
    sprintf(buffer_char, "%.2f kW", (this->evccAPIData.loadPointData[0].chargePower / 1000.0));
    Paint_DrawString_EN(12, 134, buffer_char, &Font16, WHITE, BLACK);

    // Geladen
    Paint_DrawString_EN(165, 120, txtchargedEnergy, &Font12, WHITE, BLACK);
    sprintf(buffer_char, "%.2f kWh", (this->evccAPIData.loadPointData[0].chargedEnergy / 1000.0));
    Paint_DrawString_EN(165, 134, buffer_char, &Font16, WHITE, BLACK);

    // Restzeit
    if (this->evccAPIData.loadPointData[0].chargeRemainingDuration == -1)
    {
        Paint_DrawString_EN(315, 120, txtchargechargeDuration , &Font12, WHITE, BLACK);
        this->getTime(this->evccAPIData.loadPointData[0].chargeDuration, buffer_char);
        Paint_DrawString_EN(315, 134, buffer_char, &Font16, WHITE, BLACK);
    }
    else
    {
        Paint_DrawString_EN(315, 120, txtchargeRemainingDuration, &Font12, WHITE, BLACK);
        this->getTime(this->evccAPIData.loadPointData[0].chargeRemainingDuration, buffer_char);
        Paint_DrawString_EN(315, 134, buffer_char, &Font16, WHITE, BLACK);
    }

    // little Car Logo
    Paint_DrawImage(image_car, 12, 72, 40, 28);
    // end of top half content

    epd_4in2b_v2.SendHalfBimage(0, Image);

    // 2.Draw black image bottom half
    Paint_Clear(WHITE);
    // bottom half content here...

    // Empty Rectangle for loading status
    Paint_DrawRectangle(50, 15, 360, 55, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Rectangle for Loadingstatus and targetSoc (in black if vehicleCapacity >= vehicleSoC )
    if (this->evccAPIData.loadPointData[0].vehicleSoC >= this->evccAPIData.loadPointData[0].minSoC)
    {
        Paint_DrawRectangle(52, 17, percentToXposition(this->evccAPIData.loadPointData[0].vehicleSoC), 54, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    int positionX = percentToXposition(this->evccAPIData.loadPointData[0].targetSoC);
    Paint_DrawRectangle(positionX - 4, 13, positionX + 4, 58, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Ladestand
    Paint_DrawString_EN(12, 65, txtvehicleCapacity, &Font16, WHITE, BLACK);
    sprintf(buffer_char, "%d %s", this->evccAPIData.loadPointData[0].vehicleSoC, "%");
    Paint_DrawString_EN(12, 80, buffer_char, &Font16, WHITE, BLACK);
    sprintf(buffer_char, "%d %s", this->evccAPIData.loadPointData[0].vehicleRange, "km");
    Paint_DrawString_EN(12, 95, buffer_char, &Font16, WHITE, BLACK);
    // Ladeziel
    Paint_DrawString_EN(310, 65, txttargetSoC, &Font16, WHITE, BLACK);
    sprintf(buffer_char, "%d %s", this->evccAPIData.loadPointData[0].targetSoC, "%");
    Paint_DrawString_EN(310, 80, buffer_char, &Font16, WHITE, BLACK);
    // Modus
    sprintf(buffer_char, "%s: %s", txtmode, this->evccAPIData.loadPointData[0].mode);
    Paint_DrawString_EN(190, 120, buffer_char, &Font20, WHITE, BLACK);

    // First half of EVCC Logo
    Paint_DrawImage(image_evcc_1, 12, 110, 121, 34);
    // Strike in black - not loading
    if (not this->evccAPIData.loadPointData[0].charging)
    {
        Paint_DrawImage(image_strike, 12, 15, 24, 40);
    }
    // end of bottom half content
    epd_4in2b_v2.SendHalfBimage(1, Image);

    // 3.Draw red image top half
    Paint_Clear(WHITE);
    // top half content here...

    // end of top half content
    epd_4in2b_v2.SendHalfRYimage(0, Image);

    // 4.Draw red image bottom half
    Paint_Clear(WHITE);
    // bottom half content here...

    // Ladestand - in red due to vehicleCapacity < vehicleSoC
    if (this->evccAPIData.loadPointData[0].vehicleSoC < this->evccAPIData.loadPointData[0].minSoC)
    {
        Paint_DrawRectangle(52, 17, percentToXposition(this->evccAPIData.loadPointData[0].vehicleSoC), 54, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    }
    // Strike in red while loading
    if (this->evccAPIData.loadPointData[0].charging)
    {
        Paint_DrawImage(image_strike, 12, 15, 24, 40);
    }
    // rest of EVCC Logo
    Paint_DrawImage(image_evcc_2, 63, 110, 17, 34);

    // end of bottom half content
    epd_4in2b_v2.SendHalfRYimage(1, Image);

    free(Image);
    Image = NULL;
}

void EVCCMonitor::getTime(long long Duration, char *outStr)
{
    long long chargeRemainingDuration_hours = Duration / 3600000000000;
    long long chargeRemainingDuration_min = Duration / 60000000000 - chargeRemainingDuration_hours * 60;

    sprintf(outStr, "%lld:%lld h", chargeRemainingDuration_hours, chargeRemainingDuration_min);
}