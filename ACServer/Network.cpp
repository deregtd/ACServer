#include "stdafx.h"

//#include "Network.h"
#include "FixedPackets.h"

cACServer::cACServer()
{
	//Constructor

	Listening = false;

	WSADATA wsaData;
	WORD wVersionRequested = 0x0202;
	int err;

	err = WSAStartup(wVersionRequested, &wsaData); //for winsock startup

	for (int i=0;i<128;i++)
	{
		ConnUser[i].Connected = false;
		ConnUser[i].State = 0;
		ConnUser[i].Char = 0;
	}

	MaxUsers = 0;
	NumConnected = 0;

	NumStored = 0;
	for (i=0;i<16384;i++)
		IPStore[i].Enabled = false;

	NeedToDelete = true;

	LoadOptions();

//	CalcNextFreeGUID();
}

cACServer::~cACServer()
{
	//Destructor
	SaveObjects();

	for (int i=0;i<128;i++)
		if (ConnUser[i].Connected)
			DisconnectUser(i);

	WSACleanup(); // Always do this at the end of a winsock program
}

void cACServer::LoadOptions()
{
	HKEY keyout;
	RegCreateKey(HKEY_CURRENT_USER, "Software\\ACServer\\Server", &keyout);

	long outvallen;
	outvallen = 2048;
	if (RegQueryValue(keyout, "MOTD", MOTDb, &outvallen) != ERROR_SUCCESS)
	{
		sprintf(MOTDb, "This server's operator needs to set its MOTD in the options...\n");
	}
	outvallen = 64;
	if (RegQueryValue(keyout, "ServerName", ServerName, &outvallen) != ERROR_SUCCESS)
	{
		sprintf(ServerName, "Unnamed Server");
	}
	outvallen = 4;
	if (RegQueryValueEx(keyout, "ListenPort", 0, NULL, (BYTE *) &ListenPort, (DWORD *) &outvallen) != ERROR_SUCCESS)
	{
		ListenPort = 9002;
	}
	outvallen = 16;
	if (RegQueryValue(keyout, "Speed", Speed, &outvallen) != ERROR_SUCCESS)
	{
		sprintf(Speed, "Unset Speed");
	}
	outvallen = 64;
	if (RegQueryValue(keyout, "Admin", Admin, &outvallen) != ERROR_SUCCESS)
	{
		sprintf(Admin, "Unset Admin");
	}
	outvallen = 64;
	if (RegQueryValue(keyout, "Host", Host, &outvallen) != ERROR_SUCCESS)
	{
		sprintf(Host, "");
	}
	outvallen = 4;
	if (RegQueryValueEx(keyout, "Private", 0, NULL, (BYTE *) &Private, (DWORD *) &outvallen) != ERROR_SUCCESS)
	{
		Private = false;
	}
	outvallen = 4;
	if (RegQueryValueEx(keyout, "MOTDaShown", 0, NULL, (BYTE *) &MOTDaShown, (DWORD *) &outvallen) != ERROR_SUCCESS)
	{
		MOTDaShown = true;
	}
	
	RegCloseKey(keyout);
}

void cACServer::SaveOptions()
{
	HKEY keyout;
	RegCreateKey(HKEY_CURRENT_USER, "Software\\ACServer\\Server", &keyout);

	RegSetValue(keyout, "MOTD", REG_SZ, MOTDb, strlen(MOTDb));
	RegSetValue(keyout, "ServerName", REG_SZ, ServerName, strlen(ServerName));
	RegSetValueEx(keyout, "ListenPort", 0, REG_DWORD, (const BYTE *) &ListenPort, 4);

	RegSetValue(keyout, "Admin", REG_SZ, Admin, strlen(Admin));
	RegSetValue(keyout, "Speed", REG_SZ, Speed, strlen(Speed));
	RegSetValueEx(keyout, "Private", 0, REG_DWORD, (const BYTE *) &Private, 4);
	RegSetValueEx(keyout, "MOTDaShown", 0, REG_DWORD, (const BYTE *) &MOTDaShown, 4);

	RegSetValue(keyout, "Host", REG_SZ, Host, strlen(Host));

	RegCloseKey(keyout);
}

void cACServer::StartListen()
{
	LoadObjects();

	Listening = true;

	srand(time(NULL));

	struct sockaddr_in localAddress;	//IP and port of the local machine
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.s_addr = inet_addr(LocalIP);
	localAddress.sin_port = htons(ListenPort);

	charSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	BOOL newopt = FALSE;
	setsockopt(charSocket, SOL_SOCKET, SO_DONTLINGER, (const char *) &newopt, sizeof(newopt));
	setsockopt(charSocket, SOL_SOCKET, TCP_NODELAY, (const char *) &newopt, sizeof(newopt));
	int newbuf = 0x8000; //448;
	setsockopt(charSocket, SOL_SOCKET, SO_SNDBUF, (const char *) &newbuf, sizeof(newbuf));
	setsockopt(charSocket, SOL_SOCKET, SO_RCVBUF, (const char *) &newbuf, sizeof(newbuf));
	
	bind(charSocket, (struct sockaddr *)&localAddress, sizeof(sockaddr_in) );

	listen(charSocket, SOMAXCONN);

	sprintf(sayit, "ACServer Server Up!  Listening on port %i...", ListenPort); LogDisp(sayit);

	SendDlgItemMessage(MainWnd, IDC_CHARLIST, LB_RESETCONTENT, 0, 0);
	char tosay[100];
	sprintf(tosay, "User\tIP:Port\tStatus"); SendDlgItemMessage(MainWnd, IDC_CHARLIST, LB_ADDSTRING, 0, (WPARAM) &tosay[0]);
	for (int i=0;i<128;i++)
	{
		sprintf(tosay, "%i\tN/A\tNot Connected", i);
		SendDlgItemMessage(MainWnd, IDC_CHARLIST, LB_ADDSTRING, 0, (WPARAM) &tosay[0]);
	}

	//Set up event notification on main listen socket
	int err = WSAAsyncSelect(charSocket, MainWnd, WM_USER+1, FD_ACCEPT);
}

void cACServer::LoadAccounts()
{	
	FILE *in = fopen("Password.dat","rb");
	if (!in)
	{
		NumPasses = 0;
		return;
	}
	
	fseek(in, 0, SEEK_END);
	int filelen = ftell(in);
	fseek(in, 0, SEEK_SET);

	NumPasses = (int) (filelen/sizeof(stAccounts));

	fread(ZonePasses, filelen, 1, in);

	for (int i=0;i<NumPasses;i++)
	{
		ZonePasses[i].LoggedIn = (WORD) -1;
	}

	fclose(in);
}

void cACServer::CalcNextFreeGUID()
{
	NextFreeCharGUID = 0x50000000;
	for (int i=0;i<NumPasses;i++)
	{
		for (int j=0;j<5;j++)
		{
			if (ZonePasses[i].Chars[j] >= NextFreeCharGUID)
				NextFreeCharGUID = ZonePasses[i].Chars[j] + 1;
		}
	}
}

void cACServer::SaveAccounts()
{
	FILE *out = fopen("Password.dat","wb");
	if (!out)
	{
		MessageBox(NULL, "Error on Password File Save!  Very bad!!", "Critical Error!", MB_OK);
		return;
	}

	fwrite(ZonePasses, sizeof(stAccounts)*NumPasses, 1, out);

	fclose(out);
}


void cACServer::AcceptUser()
{
	sockaddr_in tpsa; int tpsal = sizeof(tpsa);
	SOCKET tpsck = accept(charSocket, (sockaddr *) &tpsa, &tpsal);

	if (tpsck != SOCKET_ERROR)
	{
		for (int i=0;i<128;i++)
		{
			if (!ConnUser[i].Connected)
			{
				memcpy(&ConnUser[i].sockaddy,&tpsa,sizeof(sockaddr));
				ConnUser[i].Socket = tpsck;
				ConnUser[i].m_input.clear();
				ConnUser[i].m_output.clear();

				ConnUser[i].Connected = true;

				ConnUser[i].State = -1;

				ConnUser[i].RecvState = -1;

				int err = WSAAsyncSelect(tpsck, MainWnd, WM_USER+2+i, FD_READ);

				break;
			}
		}
	}
}


void cACServer::NewRead(int UserNum)
{
	int Ret;
	for ( ;; )
	{
		if (ConnUser[UserNum].RecvState == -1)
		{
			Ret = recv(ConnUser[UserNum].Socket, (char *) &ConnUser[UserNum].CurrentHeader, sizeof(EmuHeader), NULL);
			if (Ret == SOCKET_ERROR)
			{
				int ErrRet = WSAGetLastError();
				if (ErrRet == WSAEWOULDBLOCK)
				{
					//No more data, come back later...
					return;
				} else {
					//Eep bad...  Disconnected...?
					DisconnectUser(UserNum);
					return;
				}
			}

			ConnUser[UserNum].RecvState = 0;
		} else {
			Ret = recv(ConnUser[UserNum].Socket, (char *) ConnUser[UserNum].RecvBuff, ConnUser[UserNum].CurrentHeader.payloadsize_, NULL);
			if (Ret == SOCKET_ERROR)
			{
				int ErrRet = WSAGetLastError();
				if (ErrRet == WSAEWOULDBLOCK)
				{
					//No more data, come back later...
					return;
				} else {
					//Eep bad...  Disconnected...?
					DisconnectUser(UserNum);
					return;
				}
			}

			ConnUser[UserNum].RecvState += Ret;

			if (ConnUser[UserNum].RecvState == ConnUser[UserNum].CurrentHeader.payloadsize_)
			{
				ConnUser[UserNum].RecvState = -1;

				switch (ConnUser[UserNum].CurrentHeader.type_)
				{
				case EmuHeader::Authenticate:
					AuthenticateUser(UserNum);
					break;
				case EmuHeader::Payload:
					ParseMessage(UserNum);
					break;
				}
			}
		}
	}
}


void cACServer::AuthenticateUser(int UserParsing)
{
	BYTE *RecvBuf = ConnUser[UserParsing].RecvBuff;

	//Get l/p here...
	char tpLogin[20], tpPass[20];
	memcpy(tpLogin, RecvBuf, 20);
	memcpy(tpPass, RecvBuf+20, 20);
	
	bool LoginFound = false;

	int LoginRet = 2;
	//0 = Valid Existing Login
	//1 = Valid New Login
	//2 = Incorrect Password

	//Check for existing Login by that name, or else create new one...
	for (int h=0;h<NumPasses;h++)
	{
		if (strcmp(ZonePasses[h].Login,tpLogin) == 0)
		{
			//ID Found
			LoginFound = true;
			if (strcmp(ZonePasses[h].Pass,tpPass) == 0)
			{
				//Valid Password

				sprintf(sayit, "User %s logged in.", ZonePasses[h].Login); LogDisp(sayit);
				LoginRet = 0;

				ConnUser[UserParsing].ZoneNum = h;
			}
			h = NumPasses;
		}
	}
	
	if (!LoginFound)
	{
		memcpy(ZonePasses[NumPasses].Login, tpLogin, 20);
		memcpy(ZonePasses[NumPasses].Pass, tpPass, 20);
		ZeroMemory(ZonePasses[NumPasses].Chars, sizeof(ZonePasses[NumPasses].Chars));
		ZeroMemory(ZonePasses[NumPasses].Secs2Del, sizeof(ZonePasses[NumPasses].Secs2Del));
		ZeroMemory(ZonePasses[NumPasses].Names, sizeof(ZonePasses[NumPasses].Names));
		sprintf(sayit, "New User %s logged in.", ZonePasses[NumPasses].Login); LogDisp(sayit);
		LoginRet = 1;
		ZonePasses[NumPasses].LoggedIn = -1;

		ConnUser[UserParsing].ZoneNum = NumPasses;

		NumPasses++;
		SaveAccounts();
	}

	memcpy(ConnUser[UserParsing].ZoneName, ZonePasses[ConnUser[UserParsing].ZoneNum].Login, 20);
	
	//Return LoginRet here...
	//
	//!!!
	send(ConnUser[UserParsing].Socket, (char *) &LoginRet, 4, NULL);

	if (LoginRet == 2)
	{
		//Invalid Login...

		ConnUser[UserParsing].Connected = false;

		closesocket(ConnUser[UserParsing].Socket);

		return;
	}					

	ConnUser[UserParsing].State = 1;
	
	ConnUser[UserParsing].curSeq = 1;
	ConnUser[UserParsing].EventCount = 0;

	ConnUser[UserParsing].ModelCount = 0;

	ConnUser[UserParsing].msgID = 1;

	ConnUser[UserParsing].bTime = 0;
	ConnUser[UserParsing].RecTime = 0;
	ConnUser[UserParsing].LastTime = 0;

	ConnUser[UserParsing].move_count = 1;

	ConnUser[UserParsing].SelectedItem = 0;

	ConnUser[UserParsing].PortalCount = 0;
	ConnUser[UserParsing].OverrideCount = 0;
	ConnUser[UserParsing].AnimCount = 0;
	ConnUser[UserParsing].MoveItemCount = 1;
	ConnUser[UserParsing].EquipCount = 0;
	ConnUser[UserParsing].AttackCount = 0;
	ZeroMemory(ConnUser[UserParsing].Count237, sizeof(ConnUser[UserParsing].Count237));
	ZeroMemory(ConnUser[UserParsing].Count244,3);
	ConnUser[UserParsing].Count23E = 0;
	
	ConnUser[UserParsing].Connable = true;
	ConnUser[UserParsing].ConnTimer = 0;

	UpdateCharInfo(UserParsing, "Connected...");

	if (UserParsing >= MaxUsers)
		MaxUsers = UserParsing + 1;
}


void cACServer::ParseMessage(int UserParsing)
{
	BYTE *RecvBuf = ConnUser[UserParsing].RecvBuff;

	cClientPacketHeader *tphdr = (cClientPacketHeader *) RecvBuf;
	memcpy(tphdr, RecvBuf, sizeof(cClientPacketHeader));

	int PackSize = ConnUser[UserParsing].CurrentHeader.payloadsize_;

	ConnUser[UserParsing].LastTime = ConnUser[UserParsing].RecTime;
	ConnUser[UserParsing].lastInSeq = tphdr->m_dwSequence;

	switch (tphdr->m_dwFlags)
	{
	case 0x00000002:	//Error
		sprintf(sayit, "* Error Packet - 2...");
		LogDisp(sayit); 
		break;
	case 0x00000004:	//Ping
		if (ConnUser[UserParsing].bTime < tphdr->m_bTime)
			ConnUser[UserParsing].bTime = tphdr->m_bTime;

		//Reply at some point...

		break;
	case 0x00000020:	//Disconnect
		DisconnectUser(UserParsing);
		break;
	case 0x00000040:	//Timing
		//Ignore
		break;
	case 0x00100000:	//Timing
		//Ignore
		break;
	case 0x00200000:	//Unknown
		//Ignore
		break;
	case 0x00000080:	//Connection Attempt
		if (ConnUser[UserParsing].Connable)
		{
			UpdateCharInfo(UserParsing, "Sending Initial Info...");
			
			ConnUser[UserParsing].Connable = false;
			ConnUser[UserParsing].ConnTimer = 5;
			
			SendCharList(UserParsing);
			
			UpdateCharInfo(UserParsing, "Selecting Character");
		}
		break;
	case 0x00000200:
		Parse200(UserParsing, *((DWORD *) (RecvBuf+sizeof(cClientPacketHeader))), &RecvBuf[sizeof(cClientPacketHeader) + 4], PackSize - sizeof(cClientPacketHeader) - 4);
		break;
	default:
		//Unknown Type
		char * tps = new char[1 + (PackSize-sizeof(cClientPacketHeader))*5], *tps2 = tps;
		if (tps)
		{
			for (unsigned int tpl=0; tpl < (PackSize-sizeof(cClientPacketHeader)); tpl++)
			{
				sprintf(tps2, "0x%02X ", RecvBuf+sizeof(cClientPacketHeader)+tpl);
				tps2+=5;
			}
		}
		sprintf(sayit, "In:Seq:%08X Flg:%08X Srv:%08X CRC:%08X Size:%04X Tm:%02X Table:%02X * Data: %s", tphdr->m_dwSequence, tphdr->m_dwFlags, tphdr->m_dwServerID, tphdr->m_dwCRC, tphdr->m_wTotalSize, tphdr->m_bTime, tphdr->m_bTable, tps); LogDisp(sayit);
		delete [] tps;
		break;
	}
}

void cACServer::LogDisp(char *toDisp)
{
	int numtodisp = 1 + (int) (strlen(toDisp)/90);
	char tosay[91];
	tosay[90] = 0;
	int lognum;
	for (int i=0;i<numtodisp;i++)
	{
		memcpy(&tosay[0],toDisp+(i*90),90);
		lognum = SendDlgItemMessage(MainWnd, IDC_LIST1, LB_ADDSTRING/*LB_INSERTSTRING*/, 0, (LPARAM) tosay);
	}

	SendDlgItemMessage(MainWnd, IDC_LIST1, LB_SETTOPINDEX, lognum-16, 0);
}

void cACServer::IncTimes()
{
	for (int i=0;i<MaxUsers;i++)
	{
		if (ConnUser[i].Connected)
		{
			if (!ConnUser[i].Connable)
			{
				ConnUser[i].ConnTimer--;
				if (!ConnUser[i].ConnTimer)
					ConnUser[i].Connable = true;
			}

//			ConnUser[i].bTime++;
//			if (ConnUser[i].bTime > 80)
//				ConnUser[i].bTime = 0;

			ConnUser[i].RecTime++;
			if ((ConnUser[i].RecTime - ConnUser[i].LastTime) > 30)		//If 30 seconds...
			{
				DisconnectUser(i);
//				sprintf(sayit, "* User %i timed out, disconnecting...", i);  LogDisp(sayit);
			}
		}
	}

	if (NeedToDelete)
	{
		NeedToDelete = false;
		for (i=0;i<NumPasses;i++)
		{
			for (int j=0;j<5;j++)
			{
				if (ZonePasses[i].Secs2Del[j])
				{
					ZonePasses[i].Secs2Del[j]--;
					if (!ZonePasses[i].Secs2Del[j])
					{
						ZonePasses[i].Chars[j] = 0;
						ZeroMemory(ZonePasses[i].Names[j], 32);
						if (ZonePasses[i].LoggedIn != -1)
							SendCharList(ZonePasses[i].LoggedIn);
						SaveAccounts();
					} else {
						NeedToDelete = true;
					}
				}

			}
		}
	}
}

void cACServer::SendCharList(int UserParsing)
{
	BYTE PlayerList[0x2000];
	BYTE *PackPointer = PlayerList;

	DWORD tpdword;

	tpdword = 0x0000F658;			memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
	tpdword = 0x00000000;			memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
	
	int NumChars = 0;
	for (int i=0;i<5;i++)
		if (ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i])
			NumChars++;
									memcpy(PackPointer, &NumChars, 4);	PackPointer += 4;

	int outlen;
	for (i=0;i<5;i++)
	{
		if (ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i])
		{
			tpdword = ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] & 0x7FFFFFFF;
				memcpy(PackPointer,&tpdword,4); PackPointer += 4;	//GUID

			GenString(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[i], &outlen, PackPointer); PackPointer += outlen;

			//Time remaining till perma-delete
			if (ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] & 0x80000000)
				tpdword = ZonePasses[ConnUser[UserParsing].ZoneNum].Secs2Del[i];
			else
				tpdword = 0;

					memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
		}
	}

	tpdword = 0;
	for (i=0;i<5;i++)
		if (ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] & 0x80000000)
			tpdword++;
									memcpy(PackPointer,&tpdword,4);	PackPointer += 4;		//0 - DWORD
	tpdword = 5;					memcpy(PackPointer,&tpdword,4);	PackPointer += 4;		//5 - DWORD
	tpdword = 0xBEEFBEEF;			memcpy(PackPointer,&tpdword,4);	PackPointer += 4;		//BEEFBEEF - DWORD
	memcpy(PackPointer,&ConnUser[UserParsing].ZoneName[0],0x14);	PackPointer += 20;		//Zone Name - 20 Chars
	tpdword = 0;					memcpy(PackPointer,&tpdword,4);	PackPointer += 4;		//0 - DWORD
	Send200(PlayerList, (int) (PackPointer - PlayerList), 4, UserParsing);

//MOTD
	PackPointer = PlayerList;

	NumConnected = 0;
	for (int hh=0;hh<128;hh++)
	{
		if (ConnUser[hh].Connected)
			NumConnected++;
	}

	char tpmult[2] = { 's', 0 };
	if (NumConnected == 1)
		tpmult[0] = 0;

	if (MOTDaShown)
		sprintf(MOTDa, "You are in the world of: %s!\n\nThere are currently: %i Client%s Connected.\n\n", ServerName, NumConnected, tpmult);
	else
		sprintf(MOTDa, "");

	DWORD Zero = 0x0000F65A;				memcpy(PackPointer,&Zero,4);	PackPointer += 4;		//F65A - DWORD
	GenString(MOTDa, &outlen, PackPointer);	PackPointer += outlen;
	GenString(MOTDb, &outlen, PackPointer); PackPointer += outlen;

	Send200(PlayerList, (int) (PackPointer - PlayerList), 4, UserParsing);
}

void cACServer::Send200(BYTE *data, int datalen, WORD wUnk, int User)
{
	EmuHeader tph;
	tph.payloadsize_ = datalen;
	tph.message_ = wUnk;
	send(ConnUser[User].Socket, (char *) &tph, sizeof(tph), NULL);

//	send(ConnUser[User].Socket, (char *) data, datalen, NULL);

	for (int i=0;i<datalen;i+=448)	//Split into 448 byte chunks 4 some reason... :P
	{
//		cPacket tpp;
//		tpp.m_wSize = (datalen - i >= 448) ? 448 : (datalen - i);
//		tpp.m_pbPayload = new BYTE[tpp.m_wSize];
//		memcpy(tpp.m_pbPayload, data+i, tpp.m_wSize);
//		ConnUser[User].m_output.push_back(tpp);

		int ret = send(ConnUser[User].Socket, (char *) data+i, (datalen - i >= 448) ? 448 : (datalen - i), NULL);
		if (((datalen - i >= 448) && (ret != 448)) || ((datalen - i < 448) && (ret != (datalen - i))))
		{
			if (ret == SOCKET_ERROR)
			{
				int la = WSAGetLastError();
			} else {
//				MessageBox(MainWnd, "Buffer error happened, user is effectively disconnected!", "Doh!", MB_OK);
				LogDisp("Buffer error happened, a user has been disconnected...");
			}
		}
	}
}

void cACServer::DisconnectUser(int User)
{
	if (ConnUser[User].Connected == false)
		return;

	ConnUser[User].ObjectCache.clear();

	ConnUser[User].Connected = false;

	closesocket(ConnUser[User].Socket);

	NumConnected = 0;
	for (int hh=0;hh<128;hh++)
	{
		if (ConnUser[hh].Connected)
			NumConnected++;
	}

	ZonePasses[ConnUser[User].ZoneNum].LoggedIn = (WORD) -1;

	UpdateCharInfo(User, "Not Connected");

	//Recalc MaxUsers
	for (int i=127;i>=0;i--)
	{
		if (ConnUser[i].Connected)
		{	MaxUsers = i+1;	break; }
	}

	if (ConnUser[User].Char == 0)
		return;

	SaveCharacter(User);

	LBObjects[ConnUser[User].Char->Loc.landblock >> 16].erase(ConnUser[User].Char->GUID);

	if (ObjectList[ConnUser[User].Char->GUID])
		ObjectList.erase(ConnUser[User].Char->GUID);

	sprintf(sayit, "Player %s Disconnected!", ConnUser[User].Char->Name);
	ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);

	ConnUser[User].Char = 0;
}

void cACServer::UpdateCharInfo(int User, char * NewText)
{
	SendDlgItemMessage(MainWnd, IDC_CHARLIST, LB_DELETESTRING, User+1, 0);

	char TpStr[100];
	strcpy(ConnUser[User].Status, NewText);
	if (ConnUser[User].Connected)
	{
		sprintf(TpStr, "%i\t%i.%i.%i.%i:%i\t%s", User,
			ConnUser[User].sockaddy.sin_addr.S_un.S_un_b.s_b1,
			ConnUser[User].sockaddy.sin_addr.S_un.S_un_b.s_b2,
			ConnUser[User].sockaddy.sin_addr.S_un.S_un_b.s_b3,
			ConnUser[User].sockaddy.sin_addr.S_un.S_un_b.s_b4,
			ntohs(ConnUser[User].sockaddy.sin_port),
			NewText);
	} else {
		sprintf(TpStr, "%i\tN/A\t%s", User, NewText);
	}
	SendDlgItemMessage(MainWnd, IDC_CHARLIST, LB_INSERTSTRING, User+1, (WPARAM) &TpStr[0]);
}

void cACServer::SaveCharacter(int CharNum)
{
	if (!ConnUser[CharNum].Char) return;

	FILE *charread = fopen("CharList.dat","r+b");

	fseek(charread, 0, SEEK_END);
	int numchars = ftell(charread)/(SIZEOFCHAR+68);
	fseek(charread, 0, SEEK_SET);

	DWORD GUIDtemp;
	for (int j=0;j<numchars;j++)
	{
		fseek(charread, j*(SIZEOFCHAR+68), SEEK_SET);
		fread(&GUIDtemp, 4, 1, charread);
		if (GUIDtemp == ConnUser[CharNum].Char->GUID)
		{
			fseek(charread, j*(SIZEOFCHAR+68), SEEK_SET);

			BYTE TPlog[0x5000]; int charsize = ConnUser[CharNum].Char->Serialize(TPlog);
			fwrite(&ConnUser[CharNum].Char->GUID, 4, 1, charread);
			fwrite(ConnUser[CharNum].Char->Name, 32, 1, charread);
			fwrite(&ConnUser[CharNum].Char->Loc, sizeof(Location_t), 1, charread);
			fwrite(TPlog, charsize, 1, charread);
			
//			fwrite(ConnUser[CharNum].Char, sizeof(cWorldObject_Char), 1, charread);
			j = numchars;
		}
	}

	fclose(charread);
}

void cACServer::LoadObjects()
{
	sprintf(sayit, "Loading Objects.Dat..."); LogDisp(sayit);

	FILE *in = fopen("Objects.dat","rb");
	if (!in)
	{
		sprintf(sayit, "Objects.Dat Empty!  Initializing with an empty empty world."); LogDisp(sayit);
		return;
	}

	BYTE ObjData[4096];
	cWorldObject *tpObj = (cWorldObject *) ObjData;
	
	int errtest;

	cWorldObject *tpo;

	while (!feof(in))
	{
		DWORD ObjType;  errtest = fread(&ObjType, 4, 1, in);
	
		if (!errtest)
		{
			fclose(in);
			sprintf(sayit, "Objects.Dat Loaded!"); LogDisp(sayit);
			return;
		}

		switch (ObjType)
		{
		case OBJECT_LIFESTONE:
			tpo = new cWorldObject_LS();
			break;
		case OBJECT_OBJECTID:
			tpo = new cWorldObject_ObjectID();
			break;
		case OBJECT_PORTAL:
			tpo = new cWorldObject_Portal();
			break;
		case OBJECT_CHARACTER:
			//Ignore
			MessageBox(NULL, "Fatal Error: Character found in objects.dat...", "Fatal Error", MB_OK);
			break;
		default:
			//eep... bad.
			MessageBox(NULL, "Fatal Error: Unknown Object type in Objects.Dat.  Please Restore Objects.Dat.  Objects.Dat loading halted...", "Fatal Error", MB_OK);
			return;
		}

		if (ObjType != OBJECT_CHARACTER)
		{
			fread(&tpo->GUID, 4, 1, in);
			fread(tpo->Name, 32, 1, in);
			fread(&tpo->Loc, 32, 1, in);
			fread(&tpo->Owner, 4, 1, in);

			fread(ObjData, tpo->Size, 1, in);
			tpo->LoadFrom(ObjData);
			ObjectList[tpo->GUID] = tpo;
			LBObjects[tpo->Loc.landblock >> 16][tpo->GUID] = tpo;
		}

		if (ObjType == OBJECT_PORTAL)
			Portals[tpo->Loc.landblock >> 16][tpo->GUID] = (cWorldObject_Portal *) tpo;
	}
}

void cACServer::SaveObjects()
{
	//If no objects then don't bother :-)
	if (ObjectList.begin() == ObjectList.end())
		return;

	FILE *out = fopen("Objects.Dat","wb");
	if (!out)
	{
		//bad...
		MessageBox(NULL, "Can't open objects.dat for writing... very bad...", "Fatal Error", MB_OK);
		return;
	}
	
	BYTE SerData[4096];	int SerSize;

	for (std::map<DWORD, cWorldObject *>::iterator i = ObjectList.begin(); i != ObjectList.end(); i++)
	{
		if ((i->second) && (i->second->Type != OBJECT_CHARACTER))
		{
			fwrite(&i->second->Type, 4, 1, out);
			fwrite(&i->second->GUID, 4, 1, out);
			fwrite(i->second->Name, 32, 1, out);
			fwrite(&i->second->Loc, 32, 1, out);
			fwrite(&i->second->Owner, 4, 1, out);

			SerSize = i->second->Serialize(SerData);
			if (SerSize)
				fwrite(SerData, i->second->Size, 1, out);
		}
	}

	fclose(out);
}

void cACServer::SendLandblock(int User, WORD Landblock)
{
	int packlen;
	BYTE TpP[500];
	
	for (std::map<DWORD, cWorldObject *>::iterator i = LBObjects[Landblock].begin(); i != LBObjects[Landblock].end(); i++)
	{
		if ((i->second->Type != 0) && (i->second->GUID != ConnUser[User].Char->GUID))
		{
			packlen = i->second->GenerateCreatePacket(TpP);
			Send200(TpP, packlen, 3, User);
		}
	}
}


float GetDistance(Location_t *loc1, Location_t *loc2)
{
	DWORD landblock;
	int XX, ns, ew;
	double xc, yc, PosNSa, PosEWa, PosNSb, PosEWb;

	float Distance;

	if ((loc1->landblock & 0x0000FF00) && (loc2->landblock & 0x0000FF00))
	{
		//Interior
		Distance = fabs(sqrt(pow(loc2->x - loc1->x,2) + pow(loc2->y - loc1->y,2) +  + pow(loc2->z - loc1->z,2)))/240;
	} else {
		//Exterior
		landblock = loc1->landblock;
		xc = loc1->x;
		yc = loc1->y;

		XX = landblock & 0x3F; 
		ns = ((((landblock & 0x00FF0000) >> 16) + 1) * 8) + ((XX - 1) & 7) - 1027;
		ew = ((((landblock & 0xFF000000) >> 24) + 1) * 8) + (int) ((XX - 1) / 8) - 1027;
    
		xc = xc - ((int) (xc / 24.0f) * 24.0f);
		yc = yc - ((int) (yc / 24.0f) * 24.0f);
		PosEWa = ((float) (ew) / 10.0f) + (xc / 240.0f);
		PosNSa = ((float) (ns) / 10.0f) + (yc / 240.0f);
		
		landblock = loc2->landblock;
		xc = loc2->x;
		yc = loc2->y;

		XX = landblock & 0x3F; 
		ns = ((((landblock & 0x00FF0000) >> 16) + 1) * 8) + ((XX - 1) & 7) - 1027;
		ew = ((((landblock & 0xFF000000) >> 24) + 1) * 8) + (int) ((XX - 1) / 8) - 1027;

		xc = xc - (float) ((int) (xc / 24.0f) * 24.0f);
		yc = yc - (float) ((int) (yc / 24.0f) * 24.0f);
		PosEWb = ((float) (ew) / 10.0f) + (xc / 240.0f);
		PosNSb = ((float) (ns) / 10.0f) + (yc / 240.0f);

		Distance = fabs(sqrt(pow(PosNSa - PosNSb,2) + pow(PosEWa - PosEWb,2) + pow((loc1->z - loc2->z)/240,2)));
	}

	return(Distance);
}

