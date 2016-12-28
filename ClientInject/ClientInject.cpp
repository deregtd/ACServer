// ClientInject.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ClientInject.h"

#include <atlbase.h>
#include <stdio.h>
#include <winsock2.h>

static bool bRegisteredInAC = false;
static bool bInitialized = false;
static HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		g_hModule = hModule;
		DisableThreadLibraryCalls ( hModule );
		
		// Check out the project we're in - if it's client.exe, hook stuff up
		TCHAR szFilename[ MAX_PATH ];
		::GetModuleFileName( NULL, szFilename, MAX_PATH );
		
		LPTSTR strProcessName = ::_tcsrchr( szFilename, _T( '\\' ) );
		strProcessName[ 7 ] = _T( '\0' );
		
//		if( ::_tcsicmp( strProcessName + 1, _T( "client" ) ) == 0 )
			bRegisteredInAC = true;
	}
	
	return TRUE;
}

// one instance for all processes
#pragma data_seg( "InjectDll" )
bool g_bError = false;
HWND g_hwAC = 0;
HHOOK g_hHook = NULL;
DWORD g_dwIP = 0;
DWORD g_sConnPort = 0;
DWORD g_szUsername = 0,
g_szUsername1 = 0,
g_szUsername2 = 0,
g_szUsername3 = 0,
g_szUsername4 = 0,
g_szUsername5 = 0,
g_szUsername6 = 0,
g_szUsername7 = 0,
g_szUsername8 = 0,
g_szUsername9 = 0;
DWORD g_szPassword = 0,
g_szPassword1 = 0,
g_szPassword2 = 0,
g_szPassword3 = 0,
g_szPassword4 = 0,
g_szPassword5 = 0,
g_szPassword6 = 0,
g_szPassword7 = 0,
g_szPassword8 = 0,
g_szPassword9 = 0;
#pragma data_seg()

typedef void (*AdapterInitFn)( DWORD dwIP, char *, char *, short );

// The hook function - all it does is continue calls
LRESULT CALLBACK hookCBTProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr = ::CallNextHookEx ( g_hHook, nCode, wParam, lParam );
	
	if ( !bRegisteredInAC )
		return lr;
	
	if ( bInitialized )
		// Prevent recursion from wiping us out
		return lr;


	if (nCode != HCBT_CREATEWND)
		return lr;

	CBT_CREATEWND *cbtcw = (CBT_CREATEWND *) lParam;
	CREATESTRUCT *cs = cbtcw->lpcs;

	char gwt[256]; GetWindowText((HWND) wParam, gwt, 255);

	if (stricmp(cs->lpszName, "Error") == 0)
	{
		g_bError = true;

		HANDLE hEvent = ::OpenEvent ( EVENT_ALL_ACCESS, FALSE, _T( "ACEmuClientExecuted" ) );
		if ( hEvent != NULL )
		{
			::SetEvent ( hEvent );
			::CloseHandle ( hEvent );
		}

		g_hwAC = (HWND) wParam;

		return lr;
	}
	
	if (stricmp(cs->lpszName, "Asheron's Call") != 0)
		return lr;

	g_hwAC = (HWND) wParam;
	
	bInitialized = true;

	// Assume client adapter is in the same directly as this DLL
	TCHAR szPath[ MAX_PATH ];
	::GetModuleFileName ( g_hModule, szPath, MAX_PATH );

	LPTSTR strModuleName = ::_tcsrchr( szPath, _T( '\\' ) );
	::_tcscpy ( strModuleName + 1, _T( "ClientAdapter.dll" ) );

	HMODULE hMod = ::LoadLibrary ( szPath );
	if ( hMod != NULL )
	{
		AdapterInitFn fninit = reinterpret_cast< AdapterInitFn > ( ::GetProcAddress ( hMod, _T( "InitClientAdapter" ) ) );
		
		if ( fninit != NULL )
			fninit ( g_dwIP, reinterpret_cast< char * > ( &g_szUsername ), reinterpret_cast< char * > ( &g_szPassword ),  (short) g_sConnPort );
	} else {
		MessageBox(NULL, "Error loading CLIENTADAPTER.DLL! Make sure it's in the same directory as the login client!", "ClientInject Error", MB_OK);
	}
	
	// Otherwise, set the event indicating startup and return
	HANDLE hEvent = ::OpenEvent ( EVENT_ALL_ACCESS, FALSE, _T( "ACEmuClientExecuted" ) );
	if ( hEvent != NULL )
	{
		::SetEvent ( hEvent );
		::CloseHandle ( hEvent );
	}
	
	return lr;
}

void executeClient ( DWORD dwIP, char *szUsername, char *szPassword, short ConnPort, char *szClientPath )
{
	// Install the global hook, injecting this DLL into every other process
	HANDLE hEvent = ::CreateEvent ( NULL, TRUE, FALSE, _T( "ACEmuClientExecuted" ) );

	g_dwIP = dwIP;
	::strcpy ( reinterpret_cast< char * > ( &g_szUsername ), szUsername );
	::strcpy ( reinterpret_cast< char * > ( &g_szPassword ), szPassword );
	g_sConnPort = ConnPort;
	g_hHook = ::SetWindowsHookEx ( WH_CBT, hookCBTProc, g_hModule, 0 );
	
	if ( g_hHook == NULL )
	{
		DWORD dwError = ::GetLastError ();
	}
	
	// Locate and execute the client.exe process with a fancy command line
	TCHAR szCommandLine[ 2048 ];
	in_addr addr;
	addr.S_un.S_addr = dwIP;

	char *addrout = inet_ntoa ( addr ); 
	::_stprintf ( szCommandLine, _T( "%s -h %s -p %i" ), szClientPath, 
		addrout, ConnPort );
	
	STARTUPINFO si = { sizeof ( STARTUPINFO ), };
	PROCESS_INFORMATION pi;
	
	HKEY keyout;
	RegCreateKey(HKEY_CURRENT_USER, "Software\\AcEmu\\Client", &keyout);
	if (keyout)
	{
		TCHAR szServer[ 255 ];
		sprintf(szServer, "Couldn't Execute Client");
		RegSetValue(keyout, "ExitMessage", REG_SZ, szServer, strlen(szServer));
		RegCloseKey(keyout);
	}

	char szClientPathDir[255];
	strcpy(szClientPathDir, szClientPath);
	*strrchr(szClientPathDir,'\\') = 0;
	if ( CreateProcess ( NULL, szCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, szClientPathDir, &si, &pi ) )
	{
		// Wait for client startup
		::WaitForSingleObject ( hEvent, INFINITE );

		::CloseHandle ( pi.hThread );
		::CloseHandle ( pi.hProcess );
	} else {
		MessageBox(NULL, "Error Launching Client", "Login Client", MB_OK);
		::UnhookWindowsHookEx( g_hHook );
		g_hHook = NULL;
		return;
	}

	// Cleanup
	::UnhookWindowsHookEx( g_hHook );
	g_hHook = NULL;

/*	if (g_bError)
	{
		char thisdir[255], backtologin[255];
		GetModuleFileName(g_hModule, thisdir, 255);
		*strrchr(thisdir,'\\') = 0;
		sprintf(backtologin, "%s\\ClientLogon.exe",thisdir);
		WinExec(backtologin, SW_SHOW);

		return;
	}
	*/

	Sleep(1);
	
	if (g_bError)
	{
		for ( ;; )
		{
			char lala[256];
			int tpw = GetWindowText(g_hwAC, lala, 255);
			if (!tpw)
				break;
			Sleep(500);
		}
	} else {
		for ( ;; )
		{
			char lala[256];
			int tpw = GetClassName(g_hwAC, lala, 255);
			if (!tpw)
				break;
			Sleep(500);
		}
	}
	
	if (!g_bError)
	{
		//Read exit message from client
		RegCreateKey(HKEY_CURRENT_USER, "Software\\AcEmu\\Client", &keyout);
		if (keyout)
		{
			TCHAR szServer[ 255 ]; long dwSize = 255;
			if (RegQueryValue(keyout, "ExitMessage", szServer, &dwSize) == ERROR_SUCCESS)
			{
				if (stricmp(szServer, "Success") != 0)
					MessageBox(NULL, szServer, "Client Message", MB_OK | MB_ICONEXCLAMATION);
			}
			
			RegCloseKey(keyout);
		}
	}

	char thisdir[255], backtologin[255];
	GetModuleFileName(g_hModule, thisdir, 255);
	*strrchr(thisdir,'\\') = 0;
	sprintf(backtologin, "%s\\ClientLogon.exe",thisdir);
	WinExec(backtologin, SW_SHOW);
}
