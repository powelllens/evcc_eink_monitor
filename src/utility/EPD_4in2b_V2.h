/*****************************************************************************
* | File      	:   EPD_4in2b_V2.h
* | Author      :   Waveshare team
* | Function    :   4.2inch e-paper b V2
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-11-27
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __EPD_4IN2B_V2_H_
#define __EPD_4IN2B_V2_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_4IN2B_V2_WIDTH 400
#define EPD_4IN2B_V2_HEIGHT 300

// -- State of the Thing
#define EPD_4IN2B_V2_STATE_BOOT 0
#define EPD_4IN2B_V2_STATE_READY 1
#define EPD_4IN2B_V2_STATE_RESET 2
#define EPD_4IN2B_V2_STATE_READBUSY 3
#define EPD_4IN2B_V2_STATE_TURNONDISPLAY 4


class EPD_4IN2B_V2
{
public:
    byte getState(){return this->_state;};
    byte InitState = 0;
    bool getInitState(){return this->InitFinished;};
    bool getReady(){return this->_state == EPD_4IN2B_V2_STATE_READY;};
    void Init(void);
    void Clear(void);
    void Display(const UBYTE *blackimage, const UBYTE *ryimage);
    void SendHalfBimage(char TopOrBottom, const UBYTE *image);
    void SendHalfRYimage(char TopOrBottom, const UBYTE *image);
    void Sleep(void);
    void TurnOnDisplay(void);
    void doLoop(void);

private:
    void Reset(void);
    void SendCommand(UBYTE Reg);
    void SendData(UBYTE Data);
    void ReadBusy(void);
    byte _state = EPD_4IN2B_V2_STATE_BOOT;
    byte ResetSate = 0;
    byte ReadBusyState = 0;
    byte TurnOnDisplayState = 0;
    
    bool InitFinished = false;
    unsigned long lastTime = 0;
    unsigned long timerDelay = 0;
};

#endif
