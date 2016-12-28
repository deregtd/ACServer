// maindlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINDLG_H__4B488390_CE2F_4798_8415_47500D263534__INCLUDED_)
#define AFX_MAINDLG_H__4B488390_CE2F_4798_8415_47500D263534__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\ClientInject\ClientInject.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <commdlg.h>

HINSTANCE g_hInst;

class CMainDlg : public CDialogImpl<CMainDlg>,
public CMessageFilter
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}
	
	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BROWSE_CLIENT, BN_CLICKED, OnClickedBrowse_client)
		END_MSG_MAP()
		
		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();
		
		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);
		
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		
		// Lookup saved values in the registry
		CRegKey rk;
		if ( rk.Create ( HKEY_CURRENT_USER, _T( "Software\\AcEmu\\Client" ) ) != ERROR_SUCCESS )
		{
			CloseDialog ( IDCANCEL );
			return FALSE;
		}
		
		TCHAR szServer[ 255 ];
		DWORD dwSize = 255;
		
		enum eFirstFocus { eNone, eServer, eLogin, ePassword, eClientPath };
		eFirstFocus ff = eNone;
		
		if ( rk.QueryValue ( szServer, _T( "Server" ), &dwSize ) == ERROR_SUCCESS )
			SetDlgItemText ( IDC_SERVER, szServer );
		else
		{
			SetDlgItemText ( IDC_SERVER, "Server:IP" );
			ff = eServer;
		}

		TCHAR szUserName[ 255 ];
		DWORD dwUNSize = 255;
		
		if ( rk.QueryValue ( szUserName, _T( "Account" ), &dwUNSize ) == ERROR_SUCCESS )
			SetDlgItemText ( IDC_ACCOUNT, szUserName );
		else if ( ff == eNone )
			ff = eLogin;
		
		TCHAR szPassword[ 255 ];
		DWORD dwPWDSize = 255;
		
		if ( rk.QueryValue ( szPassword, _T( "Password" ), &dwPWDSize ) == ERROR_SUCCESS )
		{
			SetDlgItemText ( IDC_PASSWORD, szPassword );
			CheckDlgButton ( IDC_SAVEPASSWORD, BST_CHECKED );
		}
		else if ( ff == eNone )
			ff = ePassword;

		TCHAR szClientPath[ 255 ];
		DWORD dwCPSize = 255;
		
		if ( rk.QueryValue ( szClientPath, _T( "ClientPath" ), &dwCPSize ) == ERROR_SUCCESS )
			SetDlgItemText ( IDC_CLIENT_PATH, szClientPath );
		else if ( ff == eNone )
			ff = eClientPath;
		
		switch ( ff )
		{
		case eNone:
		case eServer:
			::SetFocus ( GetDlgItem ( IDC_SERVER ) );
			break;
			
		case eLogin:
			::SetFocus ( GetDlgItem ( IDC_ACCOUNT ) );
			break;
			
		case ePassword:
			::SetFocus ( GetDlgItem ( IDC_PASSWORD ) );
			break;

		case eClientPath:
			::SetFocus ( GetDlgItem ( IDC_CLIENT_PATH ) );
			break;
		}
		
		return FALSE;
	}
	
	in_addr m_addr;
	
	bool verifyHostName ( LPCTSTR szHostName )
	{
		WSADATA wd;
		WSAStartup ( MAKEWORD(2, 0), &wd );
		
		DWORD dwAddr = inet_addr ( szHostName );
		if ( dwAddr != INADDR_NONE )
		{
			m_addr.S_un.S_addr = dwAddr;
			
			WSACleanup ();
			return true;
		}
		
		// Do a DNS Lookup
		hostent *host_lookup = gethostbyname ( szHostName );
		
		bool bRet = ( host_lookup != NULL );
		if ( bRet )
			m_addr.S_un.S_addr = *reinterpret_cast< DWORD * > ( host_lookup->h_addr_list[ 0 ] );
		else
			MessageBox ( _T( "Host not found." ), _T( "Login Client" ), MB_ICONERROR );
		
		WSACleanup ();
		return bRet;
	}
	
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		TCHAR szComputerName[ 255 ];
		GetDlgItemText ( IDC_SERVER, szComputerName, 255 );
		
		if ( szComputerName[ 0 ] == _T( '\0' ) )
		{
			MessageBox ( _T( "Server Name is required." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_SERVER ) );
			return 0;
		}
		
		TCHAR szUserName[ 255 ];
		GetDlgItemText ( IDC_ACCOUNT, szUserName, 255 );
		
		if ( szUserName[ 0 ] == _T( '\0' ) )
		{
			MessageBox ( _T( "Account Name is required." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_ACCOUNT ) );
			return 0;
		}
		
		TCHAR szPassword[ 255 ];
		GetDlgItemText ( IDC_PASSWORD, szPassword, 255 );
		
		if ( szPassword[ 0 ] == _T( '\0' ) )
		{
			MessageBox ( _T( "Password is required." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_PASSWORD ) );
			return 0;
		}

		TCHAR szClientPath[ 255 ];
		GetDlgItemText ( IDC_CLIENT_PATH, szClientPath, 255 );
		
		if ( szPassword[ 0 ] == _T( '\0' ) )
		{
			MessageBox ( _T( "Client Path is required." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_CLIENT_PATH ) );
			return 0;
		}

		DWORD sPort = 0;
		char *szPortArea = strchr(szComputerName,':');
		if (!szPortArea)
		{
			MessageBox ( _T( "Address format is Address:Port." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_PASSWORD ) );
			return 0;
		}
		sscanf(szPortArea+1,"%i",&sPort);
		if ((!sPort) || (sPort > 32767))
		{
			MessageBox ( _T( "Invalid Port." ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_PASSWORD ) );
			return 0;
		}

/*		char tpcheckad[255]; sprintf(tpcheckad, "%s", szClientPath);
		int tpcheck = _open(tpcheckad, _O_RDONLY);
		if (tpcheck == -1)
		{
			//File doesn't exist or other error
			char invpath[255]; sprintf(invpath, "Invalid Client Path: %s", szClientPath);
			MessageBox ( _T( invpath ), _T( "Login Client" ), MB_ICONERROR );
			::SetFocus ( GetDlgItem ( IDC_CLIENT_PATH ) );
			return 0;
		}
		_close(tpcheck);
*/
		*szPortArea = 0;
		
		// Verify the computer name and convert to IP as neccessary
		{
			// CWaitCursor wc;
			if ( !verifyHostName ( szComputerName ) )
			{
				::SetFocus ( GetDlgItem ( IDC_SERVER ) );
				return 0;
			}
		}

		*szPortArea = ':';
		
		// Save Settings
		CRegKey rk;
		if ( rk.Create ( HKEY_CURRENT_USER, _T( "Software\\ACEmu\\Client" ) ) == ERROR_SUCCESS )
		{
			rk.SetValue ( szComputerName, _T( "Server" ) );
			rk.SetValue ( szUserName, _T( "Account" ) );
			
			if ( IsDlgButtonChecked ( IDC_SAVEPASSWORD ) == BST_CHECKED )
				rk.SetValue ( szPassword, _T( "Password" ) );
			else
				rk.DeleteValue ( _T( "Password" ) );

			rk.SetValue (szClientPath, _T( "ClientPath" ) );
		}
		else
			MessageBox ( _T( "Could not save settings in registry" ), _T( "Login Client" ), MB_ICONWARNING );
		
		executeClient ( m_addr.S_un.S_addr, szUserName, szPassword, (short) sPort, szClientPath );

//		SetActiveWindow();
		
		CloseDialog(wID);
		return 0;
	}
	
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}
	
	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
	LRESULT OnClickedBrowse_client(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//Add browse client shit...

		OPENFILENAME ofn;
		ZeroMemory(&ofn,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetActiveWindow();
		ofn.hInstance = g_hInst;
		char Filters[] = "Asheron's Call Client Executables\0*.exe\0\0";
		ofn.lpstrFilter = (char *) &Filters[0];
		ofn.lpstrCustomFilter = NULL;
//Add default Filename Here!
		ofn.nFilterIndex = 1;
		char FileName[255];
		sprintf(FileName,"c:\\program files\\microsoft games\\asheron's call\\*.exe");
		ofn.lpstrFile = FileName;
		ofn.nMaxFile = 100;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = NULL;
//		ofn.lpstrInitialDir = ROMDir;
		ofn.lpstrTitle = "Find Client Executable";
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrDefExt = NULL;
		ofn.lCustData = NULL;
		ofn.lpfnHook = NULL;
		ofn.lpTemplateName = NULL;
	
		int tpb = GetOpenFileName(&ofn);
		
		SetDlgItemText ( IDC_CLIENT_PATH, ofn.lpstrFile );

//		MessageBox("Not Implemented Yet", "Login Client", MB_OK);

		return 0;
	}
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__4B488390_CE2F_4798_8415_47500D263534__INCLUDED_)
