// testLeet.h : main header file for the testLeet DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestLeetApp
// See testLeet.cpp for the implementation of this class
//

class CtestLeetApp : public CWinApp
{
public:
	CtestLeetApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
