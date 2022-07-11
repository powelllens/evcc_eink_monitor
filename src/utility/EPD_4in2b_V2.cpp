/*****************************************************************************
* | File      	:   EPD_4in2b_V2.c
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
#include "EPD_4in2b_V2.h"
#include "Debug.h"

void EPD_4IN2B_V2::doLoop(void)
{
	if ((this->InitState > 0) && (this->InitState < 7))
	{
		// Init in progress...
		this->Init();
	}

	switch (this->_state)
	{
	case EPD_4IN2B_V2_STATE_RESET:
		this->Reset();
		break;
	case EPD_4IN2B_V2_STATE_READBUSY:
		this->ReadBusy();
		break;
	case EPD_4IN2B_V2_STATE_TURNONDISPLAY:
		this->TurnOnDisplay();
		break;
	default:
		break;
	}
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
void EPD_4IN2B_V2::Reset(void)
{
	switch (this->ResetSate)
	{
	case 0:
		Debug("Start Reset");
		this->_state = EPD_4IN2B_V2_STATE_RESET;
		DEV_Digital_Write(EPD_RST_PIN, 1);
		this->timerDelay = 200;
		this->lastTime = millis();
		this->ResetSate++;
	case 1:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			Debug("reset 200ms over");
			this->ResetSate++;
		}
		else
		{
			break;
		}
	case 2:
		Debug("Wait 2ms");
		DEV_Digital_Write(EPD_RST_PIN, 0);
		this->timerDelay = 2;
		this->lastTime = millis();
		this->ResetSate++;
	case 3:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			Debug("2ms over");
			this->ResetSate++;
		}
		else
		{
			break;
		}
	case 4:
		Debug("wait 200ms");
		DEV_Digital_Write(EPD_RST_PIN, 1);
		this->timerDelay = 200;
		this->lastTime = millis();
		this->ResetSate++;
	case 5:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			Debug("200ms over");
			this->ResetSate++;
		}
		else
		{
			break;
		}
	default:
		Debug("End of Reset");
		this->_state = EPD_4IN2B_V2_STATE_READY;
		this->ResetSate = 0;
	}
}

/******************************************************************************
function :	send command
parameter:
	 Reg : Command register
******************************************************************************/
void EPD_4IN2B_V2::SendCommand(UBYTE Reg)
{
	DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_SPI_WriteByte(Reg);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
	Data : Write data
******************************************************************************/
void EPD_4IN2B_V2::SendData(UBYTE Data)
{
	DEV_Digital_Write(EPD_DC_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);
	DEV_SPI_WriteByte(Data);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_4IN2B_V2::ReadBusy(void)
{
	switch (this->ReadBusyState)
	{
	case 0:
		Debug("e-Paper busy");
		this->_state = EPD_4IN2B_V2_STATE_READBUSY;
		this->timerDelay = 50;
		this->ReadBusyState++;
		break;
	case 1:
		this->SendCommand(0x71);
		this->lastTime = millis();
		this->ReadBusyState++;
		break;
	case 2:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			if (!(DEV_Digital_Read(EPD_BUSY_PIN)))
			{
				this->ReadBusyState = 1;
				break;
			}
			this->ReadBusyState++;
			this->lastTime = millis();
		}
		break;
	default:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			this->_state = EPD_4IN2B_V2_STATE_READY;
			this->ReadBusyState = 0;
			Debug("e-Paper busy release");
		}
		break;
	}
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
void EPD_4IN2B_V2::TurnOnDisplay(void)
{
	switch (this->TurnOnDisplayState)
	{
	case 0:
		Debug("Turn On Display");
		this->_state = EPD_4IN2B_V2_STATE_TURNONDISPLAY;
		this->SendCommand(0x12); // DISPLAY_REFRESH
		this->timerDelay = 100;
		this->lastTime = millis();
		this->TurnOnDisplayState++;
		break;
	case 1:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			Debug("Turn on Wait over");
			this->TurnOnDisplayState++;
		}
		break;
	default:
		this->TurnOnDisplayState = 0;
		this->_state = EPD_4IN2B_V2_STATE_READBUSY;
		this->ReadBusy();
		break;
	}
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_4IN2B_V2::Init(void)
{
	switch (this->InitState)
	{
	case 0:
		// Trigger Reset
		Debug("Start Init delay");
		this->InitFinished = false;
		this->timerDelay = 500;
		this->InitState++;
		break;
	case 1:
		if (DEV_Check_Time(this->lastTime, this->timerDelay))
		{
			Debug("Start Init delay over");
			this->InitState++;
		}
		break;
	case 2:
		// Trigger Reset
		Debug("Start Init");
		this->InitFinished = false;
		this->Reset();
		this->InitState++;
		break;
	case 3:
		// Wait for Reset to finish
		if (this->_state == EPD_4IN2B_V2_STATE_READY)
		{
			Debug("Init - Reset OK");
			this->InitState++;
		}
		break;
	case 4:
		Debug("Send cmd + busy");
		this->SendCommand(0x04);
		this->ReadBusy();
		this->InitState++;
		break;
	case 5:
		// Wait for Busy
		if (this->_state == EPD_4IN2B_V2_STATE_READY)
		{
			Debug("Busy ok");
			this->InitState++;
		}
		break;
	default:
		this->SendCommand(0x00);
		this->SendData(0x0f);
		this->InitFinished = true;
		this->InitState = 0;
		Debug("init ok");
		break;
	}
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_4IN2B_V2::Clear(void)
{
	UWORD Width, Height;
	Width = (EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1);
	Height = EPD_4IN2B_V2_HEIGHT;

	this->SendCommand(0x10);
	for (UWORD j = 0; j < Height; j++)
	{
		for (UWORD i = 0; i < Width; i++)
		{
			this->SendData(0xFF);
		}
	}

	this->SendCommand(0x13);
	for (UWORD j = 0; j < Height; j++)
	{
		for (UWORD i = 0; i < Width; i++)
		{
			this->SendData(0xFF);
		}
	}

	this->TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_4IN2B_V2::Display(const UBYTE *blackimage, const UBYTE *ryimage)
{
	UWORD Width, Height;
	Width = (EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1);
	Height = EPD_4IN2B_V2_HEIGHT;

	this->SendCommand(0x10);
	for (UWORD j = 0; j < Height; j++)
	{
		for (UWORD i = 0; i < Width; i++)
		{
			this->SendData(blackimage[i + j * Width]);
		}
	}

	this->SendCommand(0x13);
	for (UWORD j = 0; j < Height; j++)
	{
		for (UWORD i = 0; i < Width; i++)
		{
			this->SendData(ryimage[i + j * Width]);
		}
	}
}

/******************************************************************************
function :	Sends the image buffer Half image
parameter:
******************************************************************************/
void EPD_4IN2B_V2::SendHalfBimage(char TopOrBottom, const UBYTE *image)
{
	UWORD Width, Height;
	Width = (EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1);
	Height = EPD_4IN2B_V2_HEIGHT;

	if (TopOrBottom == 0)
	{ // the top half screen
		this->SendCommand(0x10);
		Debug("send the top half black screen");
		for (UWORD j = 0; j < Height / 2; j++)
		{
			for (UWORD i = 0; i < Width; i++)
			{
				this->SendData(image[i + j * Width]);
			}
		}
	}
	else if (TopOrBottom == 1)
	{ // the Bottom half screen
		Debug("send the bottom half black screen");
		for (UWORD j = 0; j < Height / 2; j++)
		{
			for (UWORD i = 0; i < Width; i++)
			{
				this->SendData(image[i + j * Width]);
			}
		}
	}
}

void EPD_4IN2B_V2::SendHalfRYimage(char TopOrBottom, const UBYTE *image)
{
	UWORD Width, Height;
	Width = (EPD_4IN2B_V2_WIDTH % 8 == 0) ? (EPD_4IN2B_V2_WIDTH / 8) : (EPD_4IN2B_V2_WIDTH / 8 + 1);
	Height = EPD_4IN2B_V2_HEIGHT;

	if (TopOrBottom == 0)
	{ // the top half screen
		this->SendCommand(0x13);
		Debug("send the top half red screen");
		for (UWORD j = 0; j < Height / 2; j++)
		{
			for (UWORD i = 0; i < Width; i++)
			{
				this->SendData(image[i + j * Width]);
			}
		}
	}
	else if (TopOrBottom == 1)
	{ // the Bottom half screen
		Debug("send the bottom half red screen");
		for (UWORD j = 0; j < Height / 2; j++)
		{
			for (UWORD i = 0; i < Width; i++)
			{
				this->SendData(image[i + j * Width]);
			}
		}
	}
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_4IN2B_V2::Sleep(void)
{
	this->SendCommand(0X50);
	this->SendData(0xf7); // border floating

	this->SendCommand(0X02); // power off
	this->ReadBusy();		 // waiting for the electronic paper IC to release the idle signal
	this->SendCommand(0X07); // deep sleep
	this->SendData(0xA5);
}
