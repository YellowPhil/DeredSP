// myprovider.h : main header file for the myprovider DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CmyproviderApp
// See myprovider.cpp for the implementation of this class
//

class CmyproviderApp : public CWinApp
{
public:
	CmyproviderApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
