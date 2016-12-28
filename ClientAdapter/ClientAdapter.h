
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLIENTADAPTER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLIENTADAPTER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLIENTADAPTER_EXPORTS
#define CLIENTADAPTER_API __declspec(dllexport)
#else
#define CLIENTADAPTER_API __declspec(dllimport)
#endif

extern "C" CLIENTADAPTER_API void InitClientAdapter ( DWORD dwIP, char *username, char *password, short ConnPort );

