/////////////////////////////////////////////////////////////////////////////
// Name:        IOConfigGenerator.cpp
// Purpose:     I/O configuration C file generator for PLC runtime
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

/*
Analog outputs are 8 bits on Arduino for the RP2040
It is not necessary to set them as outputs in the initialization function
*/

#include "IOConfigGenerator.h"

//! returns true if at least one analog input is used on the project
bool CheckAnalogInputs (TBEREMIZ4PICO_PROJECT* Project)
{
	for (int AnalogPin=0; AnalogPin<MAX_IO_PINS; AnalogPin++)
	{
		if (Project->PinUsage[AnalogPin]==IO_FUNC_ANALOG_INPUT) return true;
	}

	return false;
}  // CheckAnalogInputs
// -----------------------------------------------------------

//! Returns true if at least one analog output is used on the project
bool CheckAnalogOutputs (TBEREMIZ4PICO_PROJECT* Project)
{
	for (int Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin] == IO_FUNC_ANALOG_OUTPUT) return true;
	}

	return false;
}  // CheckAnalogOutputs
// -----------------------------------------------------------

//! Returns true if at least one servo output is used in the project
bool CheckServoOutputs (TBEREMIZ4PICO_PROJECT* Project)
{
	for (int Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin] == IO_FUNC_SERVO_OUTPUT) return true;
	}

	return false;
}  // CheckServoOutputs
// -----------------------------------------------------------

int GenerateIOConfigurationForPicoRuntime (TBEREMIZ4PICO_PROJECT* Project, wxString CFilePath)
{
	FILE* IOFile;
	wxString SourceLine;
	int Pin;
	bool AnalogInputsUsed;
	bool AnalogOutputsUsed;
	bool ServoOutputsUsed;

	AnalogInputsUsed = CheckAnalogInputs(Project);
	AnalogOutputsUsed = CheckAnalogOutputs(Project);
	ServoOutputsUsed = CheckServoOutputs(Project);

	// Try to create the file
	IOFile = fopen (CFilePath, "wt");
	if (IOFile==0)
	{
		return -1;		// Can not create the file
	}

	// Write the various constant parts in the file
	// TODO : move this part in a function
	fprintf (IOFile, "/**********************************************************************/\n");
	fprintf (IOFile, "/* This file has been generated automatically by Beremiz4Pico Manager */\n");
	fprintf (IOFile, "/* WARNING : DO NOT EDIT MANUALLY THIS FILE                           */\n");
	fprintf (IOFile, "/**********************************************************************/\n");
	fprintf (IOFile, "\n");
	fprintf (IOFile, "#include \"iec_types.h\"\n");
	fprintf (IOFile, "#include <Arduino.h>\n");
	fprintf (IOFile, "#include <Servo.h>\n");
	fprintf (IOFile, "\n");

	fprintf (IOFile, "uint8_t DigitalInput[30];\n");
	fprintf (IOFile, "uint8_t DigitalOutput[30];\n");
	if (AnalogInputsUsed)
	{
		fprintf (IOFile, "uint16_t AnalogInput[4];\n");
	}
	if (AnalogOutputsUsed)
	{  // TODO : we can optimize this by counting the real number of analog outputs
		// However, this will make difficult to optimize connections between variables and physical outputs
		// For now, we simply create the 16 PWM outputs available on Pico (PWM channels)
		fprintf (IOFile, "uint8_t AnalogOutput[16];\n");
	}
	if (ServoOutputsUsed)
	{  // TODO : we can optimize this by counting the real number of servo outputs
		// However, this will make difficult to optimize connections between variables and physical outputs
		// For now, we simply create the 16 PWM outputs available on Pico (PWM channels)
		fprintf (IOFile, "uint8_t ServoOutput[16];\n");
	}

	fprintf (IOFile, "\n");

	// Generate variable declaration depending on declared I/O pins
	for (Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_INPUT) 
		{
			SourceLine="IEC_BOOL* __IX0_"+wxString::Format(wxT("%d"), Pin)+"=&DigitalInput["+wxString::Format(wxT("%d"), Pin)+"];\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_OUTPUT)
		{
			SourceLine="IEC_BOOL* __QX0_"+wxString::Format(wxT("%d"), Pin)+"=&DigitalOutput["+wxString::Format(wxT("%d"), Pin)+"];\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_ANALOG_OUTPUT)
		{
			SourceLine="IEC_USINT* __QB0_"+wxString::Format(wxT("%d"), Pin)+"=&AnalogOutput["+wxString::Format(wxT("%d"), Pin)+"];\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_ANALOG_INPUT)
		{
			SourceLine="IEC_UINT* __IW0_"+wxString::Format(wxT("%d"), Pin)+"=&AnalogInput["+wxString::Format(wxT("%d"), Pin-26)+"];\n";   // Pins 26 to 29 are analog inputs 0 to 3
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_SERVO_OUTPUT)
		{
			SourceLine="IEC_BYTE* __QB0_"+wxString::Format(wxT("%d"), Pin)+"=&ServoOutput["+wxString::Format(wxT("%d"), Pin)+"];\n";
			fprintf (IOFile, SourceLine);
		}
	}
	fprintf (IOFile, "\n");

	// Create as many servo objects as declared servo outputs
	if (ServoOutputsUsed)
	{
		for (Pin=0; Pin<MAX_IO_PINS; Pin++)
		{
			if (Project->PinUsage[Pin]==IO_FUNC_SERVO_OUTPUT)
			{
				SourceLine = "Servo __ServoOut"+wxString::Format(wxT("%d"), Pin)+";\n";
				fprintf (IOFile, SourceLine);
			}
		}
		fprintf (IOFile, "\n");
	}

	// *** Generate ConfigurePLCIO() function ***
	fprintf (IOFile, "void ConfigurePLCIO (void)\n{\n");
	for (Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_INPUT) 
		{
			SourceLine = "  pinMode ("+wxString::Format(wxT("%d"), Pin)+", INPUT);\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_OUTPUT)
		{
			SourceLine = "  pinMode ("+wxString::Format(wxT("%d"), Pin)+", OUTPUT);\n";
			fprintf (IOFile, SourceLine);
		}
		// Analog outputs do not need to be configured here (they are set automatically by the analogWrite function)
		// Analog inputs are automatically created when AnalogRead function is called
		else if (Project->PinUsage[Pin]==IO_FUNC_SERVO_OUTPUT)
		{
			SourceLine = "  __ServoOut"+wxString::Format(wxT("%d"), Pin)+".attach ("+wxString::Format(wxT("%d"), Pin)+");\n";
			fprintf (IOFile, SourceLine);
		}
	}
	fprintf (IOFile, "}\n");

	// *** Generate AcquirePLCInputs() function ***
	fprintf (IOFile, "\nvoid AcquirePLCInputs (void)\n{\n");

	for (Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_INPUT) 
		{
			SourceLine = "  DigitalInput["+wxString::Format(wxT("%d"), Pin)+"]=digitalRead("+wxString::Format(wxT("%d"), Pin)+");\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_ANALOG_INPUT) 
		{  // TODO : check if we read pin number (26 to 29) with analogRead or analog input index
			SourceLine = "  AnalogInput["+wxString::Format(wxT("%d"), Pin-26)+"]=analogRead("+wxString::Format(wxT("%d"), Pin)+");\n";		// Pins 26 to 29 are analog inputs 0 to 3
			fprintf (IOFile, SourceLine);
		}
	}

	fprintf (IOFile, "}\n");

	// *** Generate UpdatePLCOutputs() function ***
	fprintf (IOFile, "\nvoid UpdatePLCOutputs (void)\n{\n");

	for (Pin=0; Pin<MAX_IO_PINS; Pin++)
	{
		if (Project->PinUsage[Pin]==IO_FUNC_DIGITAL_OUTPUT) 
		{
			SourceLine = "  digitalWrite ("+wxString::Format(wxT("%d"), Pin)+", DigitalOutput["+wxString::Format(wxT("%d"), Pin)+"]);\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_ANALOG_OUTPUT)
		{
			SourceLine = "  analogWrite ("+wxString::Format(wxT("%d"), Pin)+", AnalogOutput["+wxString::Format(wxT("%d"), Pin)+"]);\n";
			fprintf (IOFile, SourceLine);
		}
		else if (Project->PinUsage[Pin]==IO_FUNC_SERVO_OUTPUT)
		{
			SourceLine = "  __ServoOut"+wxString::Format(wxT("%d"), Pin)+".write (ServoOutput["+wxString::Format(wxT("%d"), Pin)+"]);\n";
			fprintf (IOFile, SourceLine);
		}
	}

	fprintf (IOFile, "}\n");

	fclose (IOFile);

	return 0;
}  // GenerateIOConfigurationForPicoRuntime
// -----------------------------------------------------------

int GenerateIOConfigurationForIonoRuntime (TBEREMIZ4PICO_PROJECT* Project, wxString CFilePath)
{
	FILE* IOFile;
	wxString SourceLine;

	// Try to create the file
	IOFile = fopen (CFilePath, "wt");
	if (IOFile==0)
	{
		return -1;		// Can not create the file
	}

	// Write the various constant parts in the file
	// TODO : move this part in a function
	fprintf (IOFile, "/**********************************************************************/\n");
	fprintf (IOFile, "/* This file has been generated automatically by Beremiz4Pico Manager */\n");
	fprintf (IOFile, "/* WARNING : DO NOT EDIT MANUALLY THIS FILE                           */\n");
	fprintf (IOFile, "/**********************************************************************/\n");
	fprintf (IOFile, "\n");
	fprintf (IOFile, "#include \"iec_types.h\"\n");
	fprintf (IOFile, "#include <Arduino.h>\n");
	//fprintf (IOFile, "#include <Servo.h>\n");
	fprintf (IOFile, "\n");

	// Iono has a fixed configuration, so we simply create all variables in a fixed pattern
	// No need to optimize memory footprint, as RP2040 has a lot of RAM and there are only a few I/O images

	// *** Generate I/O image variable (contain images of physical I/Os) ***
	fprintf (IOFile, "uint8_t DigitalInput[6];\n");
	fprintf (IOFile, "uint8_t DigitalOutput[4];\n");
	fprintf (IOFile, "uint16_t AnalogInput[4];\n");
	fprintf (IOFile, "uint8_t AnalogOutput[2];\n");

	// *** Generate PLC variables (pointers to images with IEC names) ***
	fprintf (IOFile, "IEC_BOOL* __IX0_1=&DigitalInput[0];\n");
	fprintf (IOFile, "IEC_BOOL* __IX0_2=&DigitalInput[1];\n");
	fprintf (IOFile, "IEC_BOOL* __IX0_3=&DigitalInput[2];\n");
	fprintf (IOFile, "IEC_BOOL* __IX0_4=&DigitalInput[3];\n");
	fprintf (IOFile, "IEC_BOOL* __IX0_5=&DigitalInput[4];\n");
	fprintf (IOFile, "IEC_BOOL* __IX0_6=&DigitalInput[5];\n");

	fprintf (IOFile, "IEC_BOOL* __QX0_1=&DigitalOutput[0];\n");
	fprintf (IOFile, "IEC_BOOL* __QX0_2=&DigitalOutput[1];\n");
	fprintf (IOFile, "IEC_BOOL* __QX0_3=&DigitalOutput[2];\n");
	fprintf (IOFile, "IEC_BOOL* __QX0_4=&DigitalOutput[3];\n");

	fprintf (IOFile, "IEC_UINT* __IW0_1=&AnalogInput[0];\n");
	fprintf (IOFile, "IEC_UINT* __IW0_2=&AnalogInput[1];\n");
	fprintf (IOFile, "IEC_UINT* __IW0_3=&AnalogInput[2];\n");
	fprintf (IOFile, "IEC_UINT* __IW0_4=&AnalogInput[3];\n");

	fprintf (IOFile, "IEC_USINT* __QB0_1=&AnalogOutput[0];\n");
	fprintf (IOFile, "IEC_USINT* __QB0_2=&AnalogOutput[1];\n");

	// *** Generate ConfigurePLCIO() function ***
	fprintf (IOFile, "\nvoid ConfigurePLCIO (void)\n{\n");
	// Configure digital inputs if they are used (there is nothing to do for analog inputs)
	if (Project->PinUsage[0]==IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  pinMode (26, INPUT);\n");
	if (Project->PinUsage[1]==IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  pinMode (27, INPUT);\n");
	if (Project->PinUsage[2]==IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  pinMode (28, INPUT);\n");
	if (Project->PinUsage[3]==IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  pinMode (29, INPUT);\n");

	// Configure DI5 and DI6 as "high voltage" digital inputs in all cases for now (we don't support TTL mode for now)
	fprintf (IOFile, "  pinMode (24, INPUT);\n");		//DI5
	fprintf (IOFile, "  pinMode (23, INPUT);\n");		//DI6

	// Always configure digital outputs
	fprintf (IOFile, "  pinMode (16, OUTPUT);\n");		// GPIO 16 set to output to avoid TX LED to stay ON
	fprintf (IOFile, "  digitalWrite (16, 1);\n");		// Force TX to 1 to switch off the LED
	fprintf (IOFile, "  pinMode (13, OUTPUT);\n");		// DO1
	fprintf (IOFile, "  pinMode (12, OUTPUT);\n");		// DO2
	fprintf (IOFile, "  pinMode (11, OUTPUT);\n");		// DO3
	fprintf (IOFile, "  pinMode (10, OUTPUT);\n");		// DO4

	// Analog outputs do not need to be set specifically in initialization function

	fprintf (IOFile, "}\n");

	// *** Generate AcquirePLCInputs() function ***
	fprintf (IOFile, "\nvoid AcquirePLCInputs (void)\n{\n");
	if (Project->PinUsage[0] == IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  DigitalInput[0]=digitalRead(26);\n");
	else if (Project->PinUsage[0] == IO_FUNC_ANALOG_INPUT) fprintf (IOFile, "  AnalogInput[0]=analogRead(26);\n");
	if (Project->PinUsage[1] == IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  DigitalInput[1]=digitalRead(27);\n");
	else if (Project->PinUsage[1] == IO_FUNC_ANALOG_INPUT) fprintf (IOFile, "  AnalogInput[1]=analogRead(27);\n");
	if (Project->PinUsage[2] == IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  DigitalInput[2]=digitalRead(28);\n");
	else if (Project->PinUsage[2] == IO_FUNC_ANALOG_INPUT) fprintf (IOFile, "  AnalogInput[2]=analogRead(28);\n");
	if (Project->PinUsage[3] == IO_FUNC_DIGITAL_INPUT) fprintf (IOFile, "  DigitalInput[3]=digitalRead(29);\n");
	else if (Project->PinUsage[3] == IO_FUNC_ANALOG_INPUT) fprintf (IOFile, "  AnalogInput[3]=analogRead(29);\n");

	fprintf (IOFile, "  DigitalInput[4]=digitalRead(24);\n");
	fprintf (IOFile, "  DigitalInput[5]=digitalRead(23);\n");

	fprintf (IOFile, "}\n");

	// *** Generate UpdatePLCOutputs() function ***
	fprintf (IOFile, "\nvoid UpdatePLCOutputs (void)\n{\n");

	// Set relay outputs
	fprintf (IOFile, "  digitalWrite(13, DigitalOutput[0]);\n");
	fprintf (IOFile, "  digitalWrite(12, DigitalOutput[1]);\n");
	fprintf (IOFile, "  digitalWrite(11, DigitalOutput[2]);\n");
	fprintf (IOFile, "  digitalWrite(10, DigitalOutput[3]);\n");

	// Set analog outputs
	if (Project->PinUsage[10]==IO_FUNC_ANALOG_OUTPUT)		// AO1 analog output
	{
		fprintf (IOFile, "  analogWrite(8, AnalogOutput[0]);\n");
	}
	if (Project->PinUsage[11]==IO_FUNC_ANALOG_OUTPUT)	// SW LED analog output
	{
		fprintf (IOFile, "  analogWrite(14, AnalogOutput[1]);\n");
	}

	fprintf (IOFile, "}\n");

	fclose (IOFile);

	return 0;
}  // GenerateIOConfigurationForIonoRuntime
// -----------------------------------------------------------
