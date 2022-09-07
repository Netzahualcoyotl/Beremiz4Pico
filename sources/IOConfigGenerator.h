/////////////////////////////////////////////////////////////////////////////
// Name:        IOConfigGenerator.h
// Purpose:     I/O configuration C file generator for PLC runtime
// Author:      Benoit BOUCHEZ
// Modified by:
// Created:     12/08/2020
// Copyright:   (c) Benoit BOUCHEZ
/////////////////////////////////////////////////////////////////////////////

#ifndef __IOCONFIGGENERATOR_H__
#define __IOCONFIGGENERATOR_H__

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

#include "GlobalVars.h"

int GenerateIOConfigurationForPicoRuntime (TBEREMIZ4PICO_PROJECT* Project, wxString CFilePath);
int GenerateIOConfigurationForIonoRuntime (TBEREMIZ4PICO_PROJECT* Project, wxString CFilePath);

#endif