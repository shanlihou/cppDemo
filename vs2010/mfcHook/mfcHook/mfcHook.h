// mfcHook.h : main header file for the mfcHook DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CmfcHookApp
// See mfcHook.cpp for the implementation of this class
//

class CmfcHookApp : public CWinApp
{
public:
	CmfcHookApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()
};
