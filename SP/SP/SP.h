// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SP_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// SP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SP_EXPORTS
#define SP_API __declspec(dllexport)
#else
#define SP_API __declspec(dllimport)
#endif

// This class is exported from the dll
class SP_API CSP {
public:
	CSP(void);
	// TODO: add your methods here.
};

extern SP_API int nSP;

SP_API int fnSP(void);
