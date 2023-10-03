// SP.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "SP.h"


// This is an example of an exported variable
SP_API int nSP=0;

// This is an example of an exported function.
SP_API int fnSP(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CSP::CSP()
{
    return;
}
