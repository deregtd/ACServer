o// ClientLogon.cpp : main source file for ClientLogon.exe
//

#include "stdafx.h"

#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "maindlg.h"

CAppModule _Module;

void SetupRegistry();

int Run(LPTSTR lpstrCmdLine, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	if (strstr(lpstrCmdLine, "://"))
	{
		char newaddy[300];
		strcpy(newaddy, strstr(lpstrCmdLine, "://")+3);
		newaddy[strlen(newaddy)-1] = 0;
		dlgMain.SetDlgItemText ( IDC_SERVER, newaddy );
	}

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

void main( int argc, char *argv[ ], char *envp[ ] )
{
	MessageBox(NULL, "If you get this dialog please tell akilla exactly what caused it, thanks. :-)", "WTF!?", MB_OK);
}

void SetupRegistry()
{
	CRegKey rk;
	rk.Create ( HKEY_CLASSES_ROOT, _T( "acserver" ) );
	rk.Create ( HKEY_CLASSES_ROOT, _T( "acserver\\shell" ) );
	rk.Create ( HKEY_CLASSES_ROOT, _T( "acserver\\shell\\open" ) );
	rk.Create ( HKEY_CLASSES_ROOT, _T( "acserver\\shell\\open\\command" ) );
	char curdir[500]; GetModuleFileName(NULL, curdir, 500);
	char logondir[500];
	sprintf(logondir, "%s %%1", curdir);
	rk.SetValue (HKEY_CLASSES_ROOT, _T( "acserver\\shell\\open\\command" ), logondir );
	rk.SetValue (HKEY_CLASSES_ROOT, _T( "acserver" ), "", "URL Protocol");
	rk.Close();
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	g_hInst = hInstance;

	SetupRegistry();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();

	return nRet;
}
