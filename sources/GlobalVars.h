/////////////////////////////////////////////////////////////////////////////
// Name:        GlobalsVars.h
// Purpose:     Beremiz4AVR Manager global variables
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) KissBox
/////////////////////////////////////////////////////////////////////////////

#ifndef __BEREMIZ4AVRMANAGERGLOBALVARS__
#define __BEREMIZ4AVRMANAGERGLOBALVARS__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#define CPU_TYPE_UNDEF  -1
#define CPU_TYPE_PICO	0
#define CPU_TYPE_IONO_RP	1
//#define CPU_TYPE_PIXTEND_V2S	1
//#define CPU_TYPE_PIXTEND_V2L	2

// All pins are counted even RX/TX which is normally reserved for Beremiz debugger
// This will allow possible use as I/O pins for the serial port
// This also allows to have the same index in array and pin number
#define PICO_TOTAL_PINS  30		// GP0 to GP29
#define IONO_RP_TOTAL_PINS		12		// See Iono RP User's Manual (1 line added for LED)

// Possible functions for I/O pins
#define IO_FUNC_NONE			0
#define IO_FUNC_DIGITAL_INPUT	1
#define IO_FUNC_DIGITAL_OUTPUT	2
#define IO_FUNC_ANALOG_INPUT	3
#define IO_FUNC_ANALOG_OUTPUT	4
#define IO_FUNC_SERVO_OUTPUT	5

//#define MAX_DIGITAL_IO_PIN		50		// Biggest CPU is the Mega, with 48 digital lines (analog outputs are on these pins anyway...)
//#define MAX_ANALOG_INPUT_PIN	16		// Biggest CPU is the Mega, with 16 analog inputs

#define MAX_IO_PINS		50

typedef struct {
	int HardwareType;
	unsigned int PinUsage [MAX_IO_PINS];		
	//unsigned int AnalogUsage [MAX_ANALOG_INPUT_PIN];		
} TBEREMIZ4PICO_PROJECT;

extern wxString ToolsPath;
extern wxString ProjectPath;

extern wxString NotUsedStr;
extern wxString DigitalInputStr;
extern wxString DigitalOutputStr;
extern wxString AnalogInputStr;
extern wxString AnalogOutputStr;
extern wxString ServoOutputStr;

#endif