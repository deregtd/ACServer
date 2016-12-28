// ACServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ACServer.h"

cACServer *theServer;

// Global Variables:
HINSTANCE hInst;								// current instance

bool QuitNow = false, QuitDone = false;

NOTIFYICONDATA tnid; 
SOCKADDR_IN DirServer;
LPHOSTENT lpHostEntry = 0;

DWORD DirTimerID;

char ServerNameCorr[300] = { 0 };

bool RecentAction = false;

HANDLE DirThread;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	WSADATA wsaData;
	WORD wVersionRequested = 0x0202;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData); //for winsock startup

	TCHAR szTetrisWindowClass[] = "MyInvisCharWndClass";

	WNDCLASS myc; ZeroMemory(&myc, sizeof(myc));
	myc.lpszClassName = szTetrisWindowClass;
	myc.hInstance = hInst;
	myc.lpfnWndProc = InvisWndProc;
	int a = RegisterClass(&myc);
	HWND backwindow = CreateWindow(szTetrisWindowClass, _T(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInst, NULL);

	hWnd = CreateDialog(hInstance,(LPCTSTR) IDD_MAINDLG, backwindow, &WndProc);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(hInst, (LPCTSTR) IDI_NOACT));
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(hInst, (LPCTSTR) IDI_ACSERVER));

	//Shell Icon Start
        char lpszTip[30] = "ACServer!";
	BOOL res; 

	int maintimer = SetTimer(hWnd, 758, 250, NULL);

	tnid.cbSize = sizeof(NOTIFYICONDATA); 
	tnid.hWnd = hWnd; 
	tnid.uID = 4097; //uID; 
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
	tnid.uCallbackMessage = WM_USER + 1000; 
    HICON hicon = (HICON) LoadImage(hInst, (LPCSTR) IDI_NOACT, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	tnid.hIcon = hicon; 
	if (lpszTip) 
		lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip)); 
	else 
		tnid.szTip[0] = '\0'; 

	res = Shell_NotifyIcon(NIM_ADD, &tnid); 

	if (hicon) 
		DestroyIcon(hicon);
	//Shell Icon End

	//Initialize Server Class
	theServer = new cACServer;
	theServer->MainWnd = hWnd;

	srand((unsigned) time(NULL));

	char myhost[51]; myhost[50] = 0;
	gethostname(myhost, 50);
	HOSTENT *tphost = gethostbyname(myhost);

	char tpip[64]; BYTE ip[4];
	memcpy(ip, tphost->h_addr_list[0],4);
	sprintf(tpip, "%i.%i.%i.%i:%i",ip[0],ip[1],ip[2],ip[3],theServer->ListenPort);
	SetDlgItemText(hWnd, IDC_CSIP, tpip);

	int tabstops[2] = {
		30,
		120
	};
	int ret = SendDlgItemMessage(hWnd, IDC_CHARLIST, LB_SETTABSTOPS, 2, (long) &tabstops[0]);

	char tps[80];
	sprintf(tps, "ACServer - %s", theServer->ServerName);
	SetWindowText(hWnd, tps);

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ACSERVER);
	
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	SetTimer(hWnd, 0x00AC, 1000, IncTimer);

	while (!QuitNow) 
	{
		GetMessage(&msg, NULL, 0, 0);
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	KillTimer(hWnd, 0x00AC);
	KillTimer(hWnd, 758);

	return msg.wParam;
}

VOID CALLBACK IncTimer( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
	theServer->IncTimes();
}
 
DWORD WINAPI DirTimer( LPVOID lpParam )
{
	//http://www.forotech.com/scripts/dirserv.exe
	
//	lpHostEntry = gethostbyname("localhost");
	lpHostEntry = gethostbyname("www.forotech.com");
	while (!lpHostEntry)
	{
		Sleep(3000);
//		lpHostEntry = gethostbyname("localhost");
		lpHostEntry = gethostbyname("www.forotech.com");
	}
	
	ZeroMemory(&DirServer, sizeof(DirServer));
	DirServer.sin_family = AF_INET;
	DirServer.sin_port = htons(80);
	DirServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list); 
	char *tpp = ServerNameCorr;
	for (int i=0;i<strlen(theServer->ServerName);i++)
	{
		if (theServer->ServerName[i] != ' ')
		{
			*tpp = theServer->ServerName[i];
			tpp++;
		} else {
			strcat(tpp, "%20");
			tpp+=3;
		}
	}
	*tpp = 0;
				
	while (!QuitNow)
	{
		if (!theServer->Private)
		{
			//Update Directory Server
			SOCKET DirSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (connect(DirSocket, (LPSOCKADDR) &DirServer, sizeof(sockaddr)) != SOCKET_ERROR)
			{
				char HostString[80] = { 0 };
				if (strlen(theServer->Host) > 0)
					sprintf(HostString, "&Host=%s", theServer->Host);

				char constring[256];
				sprintf(constring, "GET /scripts/dirserv.exe?Name=%s&Port=%i&Type=1&Users=%i&Admin=%s&Speed=%s%s\r\n", ServerNameCorr, theServer->ListenPort, theServer->NumConnected, theServer->Admin, theServer->Speed, HostString);
				send(DirSocket, constring, strlen(constring), NULL);
		//			char lala[1000]; recv(DirSocket, lala, 1000, NULL);
			}

			closesocket(DirSocket);
		}

		for (int i=0;i<15;i++)
		{
			Sleep(1000);
			if (QuitNow)
				break;
		}
	}

	//Send Quit Message
	SOCKET DirSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(DirSocket, (LPSOCKADDR) &DirServer, sizeof(sockaddr)) != SOCKET_ERROR)
	{
		char HostString[80] = { 0 };
		if (strlen(theServer->Host) > 0)
			sprintf(HostString, "&Host=%s", theServer->Host);

		char constring[256];
		sprintf(constring, "GET /scripts/dirserv.exe?Quit=1&Port=%i&%s\r\n", theServer->ListenPort, HostString);
		send(DirSocket, constring, strlen(constring), NULL);
	}

	closesocket(DirSocket);

	QuitDone = true;

	return(0);
}

int CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	int i;

	if (message == WM_USER+1)
	{
		RecentAction = true;
		theServer->AcceptUser();
		RecentAction = true;

		return false;
	}

	if ((message >= WM_USER+2) && (message < WM_USER+1000))
	{
		RecentAction = true;
		theServer->NewRead(message - WM_USER - 2);
		RecentAction = true;

		return false;
	}

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_OPTIONS_CLEAROBJECTSLIST:
			{
				char tps[300];
				sprintf(tps, "Clearing Internal Objects List..."); theServer->LogDisp(tps);
				theServer->ObjectList.clear();
				sprintf(tps, "Deleting Objects.Dat..."); theServer->LogDisp(tps);
				DeleteFile("Objects.Dat");
				sprintf(tps, "Done!"); theServer->LogDisp(tps);
			}
			break;
		case ID_OPTIONS_CLEARUSERLIST:
			{
				char tps[300];
				sprintf(tps, "Clearing Internal User List..."); theServer->LogDisp(tps);
				theServer->NumPasses = 0;
				sprintf(tps, "Deleting Password.Dat..."); theServer->LogDisp(tps);
				DeleteFile("Password.Dat");
				sprintf(tps, "Deleting CharList.Dat..."); theServer->LogDisp(tps);
				DeleteFile("CharList.Dat");
				sprintf(tps, "Done!"); theServer->LogDisp(tps);
			}
			break;
		case ID_COMMANDS_STARTLISTEN:
			SendMessage(hWnd, WM_COMMAND, MAKELONG(IDC_STARTLISTEN, 0), 0);
			break;
		case ID_COMMANDS_SERVERMESSAGE:
			{
				HWND newdiag = CreateDialog(hInst, (LPCTSTR) IDD_SRVMSG, hWnd, SrvMsgProc);
				SendDlgItemMessage(newdiag, IDC_EDIT1, EM_SETLIMITTEXT, 1024, 0);
			}
			break;
		case ID_OPTIONS_OPENOPTIONS:
			{
				HWND newdiag = CreateDialog(hInst, (LPCTSTR) IDD_OPTIONS, hWnd, OptionsProc);
				SetDlgItemText(newdiag, IDC_EDIT1, theServer->ServerName);
				SetDlgItemInt(newdiag, IDC_EDIT2, theServer->ListenPort, false);
				char MOTDtp[2048]; strcpy(MOTDtp, theServer->MOTDb);

				for (int i=0;i<strlen(MOTDtp);i++)
				{
					if (MOTDtp[i] == '\n')
						MOTDtp[i-1] = '\r';
				}

				SetDlgItemText(newdiag, IDC_EDIT3, MOTDtp);
				
				SetDlgItemText(newdiag, IDC_EDIT4, theServer->Speed);
				SetDlgItemText(newdiag, IDC_EDIT5, theServer->Admin);

				SetDlgItemText(newdiag, IDC_EDIT6, theServer->Host);

				SendDlgItemMessage(newdiag, IDC_EDIT1, EM_SETLIMITTEXT, 64, 0);
				SendDlgItemMessage(newdiag, IDC_EDIT2, EM_SETLIMITTEXT, 5, 0);
				SendDlgItemMessage(newdiag, IDC_EDIT3, EM_SETLIMITTEXT, 2048, 0);

				SendDlgItemMessage(newdiag, IDC_EDIT4, EM_SETLIMITTEXT, 16, 0);
				SendDlgItemMessage(newdiag, IDC_EDIT5, EM_SETLIMITTEXT, 64, 0);

				SendDlgItemMessage(newdiag, IDC_EDIT6, EM_SETLIMITTEXT, 64, 0);

				SendDlgItemMessage(newdiag, IDC_CHECK1, BM_SETCHECK, BST_UNCHECKED, 0);

				if (theServer->Private)
					SendDlgItemMessage(newdiag, IDC_CHECK1, BM_SETCHECK, BST_CHECKED, 0);
				if (theServer->MOTDaShown)
					SendDlgItemMessage(newdiag, IDC_CHECK2, BM_SETCHECK, BST_CHECKED, 0);
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_RESETLOG:
			SendDlgItemMessage(hWnd, IDC_LIST1, LB_RESETCONTENT, 0, 0);
			break;
		case ID_FILE_RESETALLCONNECTIONS:
			for (i=0;i<128;i++)
				theServer->DisconnectUser(i);
			break;
		case IDC_STARTLISTEN:
			{
				theServer->LoadAccounts();
				theServer->StartListen();

				HWND tpi = GetDlgItem(hWnd, IDC_STARTLISTEN);
				ShowWindow(tpi, SW_HIDE);
				SetWindowLong(tpi, GWL_STYLE, GetWindowLong(tpi, GWL_STYLE) | WS_DISABLED);
				
				HMENU tpm = GetMenu(hWnd);
//				int GetMenuItemID(tpm, 0);
				EnableMenuItem(tpm, ID_COMMANDS_STARTLISTEN, MF_DISABLED | MF_GRAYED);
				
				DirThread = CreateThread(NULL, NULL, DirTimer, NULL, NULL, &DirTimerID);
//				SetThreadPriority(DirThread, THREAD_PRIORITY_LOWEST);
			}
			break;
		case ID_FILE_KICKSELECTEDUSER:
			i = SendDlgItemMessage(hWnd, IDC_CHARLIST, LB_GETCURSEL, 0, 0);
			if ((i != LB_ERR) && (i > 0))
				theServer->DisconnectUser(i-1);
			break;
		case ID_KICK_USER:
			i = SendDlgItemMessage(hWnd, IDC_CHARLIST, LB_GETCURSEL, 0, 0);
			if ((i != LB_ERR) && (i > 0))
				theServer->DisconnectUser(i-1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
	case WM_CONTEXTMENU:
		{
			if ((HWND) wParam == GetDlgItem(hWnd, IDC_CHARLIST))
			{
				int xPos = LOWORD(lParam);  // horizontal position of cursor 
				int yPos = HIWORD(lParam);  // vertical position of cursor 
				
				HMENU tpmenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_USERPOPUP));
				HMENU submenu = GetSubMenu(tpmenu, 0);

				TrackPopupMenu(submenu, TPM_LEFTALIGN, xPos, yPos, NULL, hWnd, NULL);
			}
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		QuitNow = true;
		{
			DWORD a; GetExitCodeThread(DirThread, &a);
			TerminateThread(DirThread, a);
		}
		delete theServer;
		Shell_NotifyIcon(NIM_DELETE, &tnid); 
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		{
			HICON hicon;
			if (RecentAction)
				hicon = (HICON) LoadImage(hInst, (LPCSTR) IDI_ACT, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
			else
				hicon = (HICON) LoadImage(hInst, (LPCSTR) IDI_NOACT, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
			tnid.hIcon = hicon;
			sprintf(tnid.szTip, "%s - %i/128", theServer->ServerName, theServer->NumConnected);

			Shell_NotifyIcon(NIM_MODIFY, &tnid); 

			if (hicon) 
				DestroyIcon(hicon);

			RecentAction = false;
		}
		break;

	case WM_USER+1000:
		switch (lParam)
		{
		case 512:
			//Mouseover
			break;
		case 513:
			//LButtondown
			{
				WINDOWPLACEMENT wp; wp.length = sizeof(wp);
				GetWindowPlacement(hWnd, &wp);
				if (wp.showCmd == SW_SHOWMINIMIZED)
				{
					ShowWindow(hWnd, SW_RESTORE);
					ShowWindow(hWnd, SW_SHOWNORMAL);
					SetForegroundWindow(hWnd);
				}
				else
				{
					if (GetForegroundWindow() != hWnd)
						SetForegroundWindow(hWnd);
				}
			}
			break;
		case 516:
			//RButtondown
			ShowWindow(hWnd, SW_MINIMIZE);
			ShowWindow(hWnd, SW_HIDE);
			break;
		}
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			SendMessage(hWnd, WM_USER+1000, 0, 516);
		break;
   }
   return false;
}

long CALLBACK InvisWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int CALLBACK OptionsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDOK:
			{
				GetDlgItemText(hWnd, IDC_EDIT4, theServer->Speed, 16);
				GetDlgItemText(hWnd, IDC_EDIT5, theServer->Admin, 64);
				GetDlgItemText(hWnd, IDC_EDIT6, theServer->Host, 64);

				int NewCheck = SendDlgItemMessage(hWnd, IDC_CHECK1, BM_GETCHECK, 0, 0);
				if (NewCheck == BST_CHECKED)
					theServer->Private = true;
				if (NewCheck == BST_UNCHECKED)
					theServer->Private = false;

				NewCheck = SendDlgItemMessage(hWnd, IDC_CHECK2, BM_GETCHECK, 0, 0);
				if (NewCheck == BST_CHECKED)
					theServer->MOTDaShown = true;
				if (NewCheck == BST_UNCHECKED)
					theServer->MOTDaShown = false;

				GetDlgItemText(hWnd, IDC_EDIT1, theServer->ServerName, 64);
				int bout;
				int OldListen = theServer->ListenPort;
				theServer->ListenPort = GetDlgItemInt(hWnd, IDC_EDIT2, &bout, false);
				if ((!bout) || (theServer->ListenPort > 32767))
					theServer->ListenPort = 9002;
				GetDlgItemText(hWnd, IDC_EDIT3, theServer->MOTDb, 2048);
				char * tpc = strchr(theServer->MOTDb,'\r');
				while (tpc)
				{
					*tpc = ' ';
//					*(tpc+1) = '\n';
					tpc = strchr(theServer->MOTDb,'\r');
				}
				theServer->SaveOptions();
				DestroyWindow(hWnd);

				char myhost[51]; myhost[50] = 0;
				gethostname(myhost, 50);
				HOSTENT *tphost = gethostbyname(myhost);

				char tpip[64]; BYTE ip[4];
				memcpy(ip, tphost->h_addr_list[0],4);
				sprintf(tpip, "%i.%i.%i.%i:%i",ip[0],ip[1],ip[2],ip[3],theServer->ListenPort);
				SetDlgItemText(theServer->MainWnd, IDC_CSIP, tpip);

				char tps[80];
				sprintf(tps, "ACServer - %s", theServer->ServerName);
				SetWindowText(theServer->MainWnd, tps);

				//Update Listen Port
				if ((theServer->Listening) && (theServer->ListenPort != OldListen))
				{
					closesocket(theServer->charSocket);

					struct sockaddr_in localAddress;	//IP and port of the local machine
					localAddress.sin_family = AF_INET;
					localAddress.sin_addr.s_addr = inet_addr(theServer->LocalIP);
					localAddress.sin_port = htons(theServer->ListenPort);

					theServer->charSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					
					bind(theServer->charSocket, (struct sockaddr *)&localAddress, sizeof(sockaddr_in) );

					listen(theServer->charSocket, SOMAXCONN);
	
					char tps[300];
					sprintf(tps, "ACServer Relistening on port %i...", theServer->ListenPort); theServer->LogDisp(tps);

					//Set up event notification on main listen socket
					int err = WSAAsyncSelect(theServer->charSocket, hWnd, WM_USER+1, FD_ACCEPT/* | FD_READ*/);
				}
			}
			break;
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}
	return false;
}

int CALLBACK SrvMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDOK:
			{
				char SrvMsg[1024];
				GetDlgItemText(hWnd, IDC_EDIT1, SrvMsg, 1024);
				theServer->ServerMessage(GLOBAL_MESSAGE, SrvMsg, COLOR_GREEN);
				DestroyWindow(hWnd);
			}
			break;
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		}
		break;
	}
	return false;
}
