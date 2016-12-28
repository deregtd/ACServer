// Socket.cpp
// Implementation of cUDPSocket - spoofer for the AC UDP protocol

#include "stdafx.h" 
#include "Socket.h"
#include <time.h>

// Send this once
// 0x1FA3CB43
/*
static DWORD verinfo2[] = {
	// Packet Header
	0x1, 0x80, 0x1FA3CB43, 0xC, MAKELONG( 102, 1 ),
		
		// Body
		0xC301F6BA, 0x418D6083, 0x00000000, 0x00000000, 0x000C0328, 0x2E350000, 0x30303032,
		0x00003730, 0x20000000, 0x003C2000, 0x77880000, 0x00083366, 0x63FB0000, 0xC71608C5, 
		0x43FF3180, 0xF38AFEE5, 0xCF2369E3, 0xC4B982F8, 0x30998059, 0xC2E508B6, 0xADBBDD80, 
		0x0003ADCD, 0x36B80000, 0x733585F1, 0x488583F4, 0x0000BBDF
};
*/

static BYTE verinfo2[] = {
	0x01, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x60, 0x94, 0x51, 0xAE, 0x0B, 0x00, 0x00, 0x00,
	0x66, 0x00, 0x00, 0x00, 0x8F, 0xFE, 0xEC, 0x29, 0xCF, 0x7F, 0x91, 0x41, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x9E, 0x02, 0x0C, 0x00, 0x00, 0x00, 0x32, 0x30, 0x30, 0x32, 0x2E, 0x30,
	0x32, 0x2E, 0x30, 0x30, 0x31, 0x00, 0x00, 0x20, 0x00, 0x20, 0x3C, 0x00, 0x00, 0x00, 0x88, 0x77,
	0x66, 0x33, 0x08, 0x00, 0x00, 0x00, 0xFB, 0x63, 0xC5, 0x08, 0x16, 0xC7, 0x80, 0x31, 0xFF, 0x43,
	0xE5, 0xFE, 0x8A, 0xF3, 0xE3, 0x69, 0x23, 0xCF, 0xF8, 0x82, 0xB9, 0xC4, 0x59, 0x80, 0x99, 0x30,
	0xB6, 0x08, 0xE5, 0xC2, 0x80, 0xDD, 0xBB, 0xAD, 0xCD, 0xAD, 0x03, 0x00, 0x00, 0x00, 0x58, 0x10,
	0x41, 0x5C, 0x2A, 0x8F, 0xFA, 0x56, 0x85, 0x48, 0xDF, 0xBB,

/*	0x01, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xDD, 0x51, 0xF8, 0x31, 0x0B, 0x00, 0x00, 0x00,   //(_________Qø1____)
	0x66, 0x00, 0x01, 0x00, 0xD6, 0x57, 0x1D, 0x4F, 0x6C, 0xD6, 0x8F, 0x41, 0x00, 0x00, 0x00, 0x00,   //(f____W_Ol__A____)
	0x00, 0x00, 0x00, 0x00, 0x54, 0x01, 0x0C, 0x00, 0x00, 0x00, 0x32, 0x30, 0x30, 0x31, 0x2E, 0x31,   //(____T_____2001.1)
	0x32, 0x2E, 0x30, 0x30, 0x32, 0x00, 0x00, 0x20, 0x00, 0x20, 0x3C, 0x00, 0x00, 0x00, 0x88, 0x77,   //(2.002__ _ <____w)
	0x66, 0x33, 0x08, 0x00, 0x00, 0x00, 0xFB, 0x63, 0xC5, 0x08, 0x16, 0xC7, 0x80, 0x31, 0xFF, 0x43,   //(f3_____c_____1ÿC)
	0xE5, 0xFE, 0x8A, 0xF3, 0xE3, 0x69, 0x23, 0xCF, 0xF8, 0x82, 0xB9, 0xC4, 0x59, 0x80, 0x99, 0x30,   //(_þ___i#_ø___Y__0)
	0xB6, 0x08, 0xE5, 0xC2, 0x80, 0xDD, 0xBB, 0xAD, 0xCD, 0xAD, 0x03, 0x00, 0x00, 0x00, 0x2F, 0x0E,   //(______________/_)
	0x7E, 0x44, 0xCA, 0xEE, 0x9F, 0x7E, 0x85, 0x48, 0xDF, 0xBB,                                       //(~D___~_H__)*/
};

static BYTE stupidconnect[] = {
0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xDE, 0xC2, 0xF2, 0x27, 0x0B,
0x00, 0x00, 0x00, 0xE6, 0x00, 0x00, 0x00, 0x01, 0xD1, 0x6F, 0x2B, 0xCF, 0x7F,
0x91, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
};

static BYTE stupid2[] = {
0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0xED, 0x9C, 0xDF, 0x99, 0x0B,
0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x17, 0x1D, 0x34, 0xCF, 0x7F,
0x91, 0x41,
};

static DWORD set_char_list_mode[] = {
	0x0000F7B8, 0x00000659, 0x00000000, 0x0000050E, 0x00000000, 0x00000000, 0x00000000, 
//	0x0000F7B8, 0x00000600, 0x00000000, 0x000004C0, 0x00000000, 0x00000000, 0x00000000,
};

static DWORD char_list[] = {
	0x0000F658, 0x00000000, 0x00000005, 0x500522EB, 0x6943000B, 0x4D206F62, 0x6F747461, 0x00000000, 
		0x00000000, 0x500532AB, 0x614D0006, 0x006F6B6B, 0x00000000, 0x50054871, 0x6954000C, 0x69726562, 
		0x75736E61, 0x0000006E, 0x00000000, 0x5006C222, 0x75420008, 0x65736E72, 0x00000079, 0x00000000, 
		0x5006C223, 0x614B0009, 0x7369656C, 0x0000616B, 0x00000000, 0x00000000, 0x00000005, 0xBEEFBEEF, 
		0x77656873, 0x616E6E61, 0x00000032, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 
};

cUDPSocket::cUDPSocket ( DWORD dwIP, const char *szUsername, const char *szPassword, short ConnPort )
: m_hInputWaiting ( ::WSACreateEvent () ),
m_hOutputWaiting ( ::WSACreateEvent () ),
m_hShutdown ( ::WSACreateEvent () ),
m_s ( ::socket ( AF_INET, SOCK_STREAM, 0 ) ),
m_dwServerSequence ( 1 ),
m_dwClientSequence ( 0 ),
m_dwFragmentSequence ( 0xE9659 )
{
	HKEY keyout;
	RegCreateKey(HKEY_CURRENT_USER, "Software\\AcEmu\\Client", &keyout);
	if (keyout)
	{
		TCHAR szServer[ 255 ];
		sprintf(szServer, "Success");
		RegSetValue(keyout, "ExitMessage", REG_SZ, szServer, strlen(szServer));
		RegCloseKey(keyout);
	}

#ifdef _DEBUG
	FILE *out = fopen("c:\\clientlog.htm","wb");
	fprintf(out, "<PRE>\n");
	fclose(out);
#endif

	m_state.m_bConnected = false;
	m_state.m_stage = eStrobe;
	
	::InitializeCriticalSection ( &m_cs );
	
	// Perform connection here synchronously
	sockaddr_in in;
	in.sin_addr.S_un.S_addr = dwIP;
	in.sin_family = AF_INET;
	in.sin_port = htons(ConnPort);

//	BOOL newopt = TRUE;
//	setsockopt(m_s, SOL_SOCKET, SO_DONTLINGER, (const char *) &newopt, sizeof(newopt));
	int newbuf = 0x2000;
	setsockopt(m_s, SOL_SOCKET, SO_SNDBUF, (const char *) &newbuf, sizeof(newbuf));
	setsockopt(m_s, SOL_SOCKET, SO_RCVBUF, (const char *) &newbuf, sizeof(newbuf));

	int nResult = ::connect ( m_s, reinterpret_cast< sockaddr * > ( &in ), sizeof ( sockaddr_in ) );

	if ( nResult == SOCKET_ERROR )
	{
		RegCreateKey(HKEY_CURRENT_USER, "Software\\AcEmu\\Client", &keyout);
		if (keyout)
		{
			TCHAR szServer[ 255 ];
			int wse = WSAGetLastError();
			if (wse == WSAETIMEDOUT)
				sprintf(szServer, "Connection Failed: Connection Timed Out...");
			else if (wse == WSAECONNREFUSED)
				sprintf(szServer, "Connection Failed: Connection Refused...");
			else if (wse == WSAEHOSTUNREACH)
				sprintf(szServer, "Connection Failed: Host Unreachable");
			else
				sprintf(szServer, "Connection Failed: Error Message %i...", wse);

			RegSetValue(keyout, "ExitMessage", REG_SZ, szServer, strlen(szServer));
			RegCloseKey(keyout);
		}

		DWORD tpec;
		GetExitCodeProcess(GetCurrentProcess(), &tpec);
		ExitProcess(tpec);
//		TerminateProcess(GetCurrentProcess(), tpec);
	}
	
	// Fill out a header and an authentication - do this synchronously
	EmuHeader header = { EmuHeader::Authenticate, 0, sizeof ( EmuAuthenticate ), 0 };
	nResult = ::send ( m_s, reinterpret_cast< char * > ( &header ), sizeof ( EmuHeader ), 0 );
	
	EmuAuthenticate auth;
	::strcpy ( auth.username_, szUsername );
	::strcpy ( auth.password_, szPassword );
	
	nResult = ::send ( m_s, reinterpret_cast< char * > ( &auth ), sizeof ( EmuAuthenticate ), 0 );
	
	// Now attempt to read back the session ID
	int ResponseType;
	nResult = ::recv ( m_s, reinterpret_cast< char * > ( &ResponseType ), 4, 0 );
	if (ResponseType == 2)
	{
		HKEY keyout;
		RegCreateKey(HKEY_CURRENT_USER, "Software\\AcEmu\\Client", &keyout);
		char szServer[50];
		sprintf(szServer, "Invalid Password...");
		RegSetValue(keyout, "ExitMessage", REG_SZ, szServer, strlen(szServer));
		RegCloseKey(keyout);
	}
	
	DWORD dwThreadID;
	m_hTCPThread = ::CreateThread ( NULL, 0, tcpThread, this, CREATE_SUSPENDED, &dwThreadID );
	
	// Thread will put the connection into non-blocking mode and begin relaying messages
}

cUDPSocket::~cUDPSocket ()
{
	::SetEvent ( m_hShutdown );
	// Make sure it's running
	::ResumeThread ( m_hTCPThread );
	::WaitForSingleObject ( m_hTCPThread, INFINITE );
	
	::CloseHandle ( m_hTCPThread );
	
	::DeleteCriticalSection ( &m_cs );
	
	::WSACloseEvent ( m_hShutdown );
	::WSACloseEvent ( m_hInputWaiting );
	::WSACloseEvent ( m_hOutputWaiting );
}

int cUDPSocket::recvfrom ( char *buf, int len, int flags, sockaddr FAR *from, int *fromlen )
{
	_ASSERTE ( flags == 0 );
	_ASSERTE ( fromlen != NULL );
	_ASSERTE ( *fromlen == sizeof ( sockaddr_in ) );
	
	// We assume that the socket is configured for non-blocking IO
	DWORD dwWait = ::WSAWaitForMultipleEvents ( 1, &m_hInputWaiting, TRUE, 0, FALSE );
	if ( dwWait == WAIT_TIMEOUT )
	{
		::WSASetLastError ( WSAEWOULDBLOCK );
		return 0;
	}
	
	::EnterCriticalSection ( &m_cs );
	
	// Pull the packet from the list
	cPacket p = m_input.front ();
	m_input.pop_front ();
	
	if ( m_input.empty () )
		::ResetEvent ( m_hInputWaiting );
	
	::LeaveCriticalSection ( &m_cs );
	
	if ( from != NULL )
		::memcpy ( from, &m_connect, sizeof ( sockaddr_in ) );
	
	int copysz = min ( len, p.m_wSize );
	::memcpy ( buf, p.m_pbPayload, copysz );
	delete[] p.m_pbPayload;
	
	return copysz;
}

void cUDPSocket::postReceivePacket ( void *buf, int len )
{
	cPacket p;
	p.m_pbPayload = new BYTE[ len ];
	memcpy ( p.m_pbPayload, buf, len );
	p.m_wSize = len;
	
	postPacket ( p );
}

void cUDPSocket::postPong ()
{
//	enum { CHECKSUM_MASK = 0xBAF770F6 };
	enum { CHECKSUM_MASK = 0xBAF970F7 };
	
	DWORD dwPong[] = {
		m_dwServerSequence,
			0x4,
			CHECKSUM_MASK + m_dwClientSequence,
			0x0005000B,
//			MAKELONG(4, 1),
			0x00030004,
			m_dwClientSequence };
		
		postReceivePacket ( dwPong, sizeof ( dwPong ) );
}

void cUDPSocket::postFragment ( void *pvPayload, WORD wClass, size_t szPayload )
{
	::EnterCriticalSection ( &m_cs );
	
	static BYTE _fragmentBuffer[ 512 ];
	static cFragmentHeader *pHeader = reinterpret_cast< cFragmentHeader * > ( _fragmentBuffer );
	
	// Fill out the header
	pHeader->m_dwSequence = m_dwFragmentSequence ++;
	pHeader->m_dwChannel = 0x80000000;
	pHeader->m_wFragmentCount = 1;
	pHeader->m_wLength = szPayload + sizeof ( cFragmentHeader );
	pHeader->m_wFragmentIndex = 0;
	pHeader->m_wUnknown = wClass;
	
	// Place the payload
	::memcpy ( pHeader + 1, pvPayload, szPayload );
	
	// Dispatch
	postAppPacket ( _fragmentBuffer, sizeof ( cFragmentHeader ) + szPayload );
	
	::LeaveCriticalSection ( &m_cs );
}

void cUDPSocket::sendFragment ( void *payload, size_t bytecount )
{
	// Prepare the fragment with the header and post immediately
	cPacket packet = { new BYTE[ bytecount ], bytecount };
	memcpy ( packet.m_pbPayload, payload, bytecount );
	
	::EnterCriticalSection ( &m_cs );
	
	if ( m_output.empty () )
		::WSASetEvent ( m_hOutputWaiting );
	m_output.push_back ( packet );
	
	::LeaveCriticalSection ( &m_cs );
}

int cUDPSocket::sendto ( const char FAR *buf, int len, int flags, const struct sockaddr FAR *to, int tolen )
{
	//   _ASSERTE ( flags == 0 );
	_ASSERTE ( tolen == sizeof ( sockaddr_in ) );
	
	// Decode the packet that's being sent, possibly handle it locally
	if ( !m_state.m_bConnected )
	{
		::memcpy ( &m_connect, to, sizeof ( sockaddr_in ) );
		m_state.m_bConnected = true;
	}
	
	_ASSERTE ( reinterpret_cast< const sockaddr_in * > ( to )->sin_addr.S_un.S_addr == m_connect.sin_addr.S_un.S_addr );
	
	const cPacketHeader *pPacket = reinterpret_cast< const cPacketHeader * > ( buf );
	
#ifdef _DEBUG
	FILE *out = fopen("c:\\clientlog.htm","ab");
	time_t tpt = time(NULL);
	struct tm *newtime = localtime(&tpt);
	char *tpstr = asctime(newtime);
	tpstr[24] = 0;
	fprintf(out, "%s: Out -> Flg:%08X - Data:", tpstr, pPacket->m_dwFlags);
	if (pPacket->m_dwFlags == 0x200)
		for (int i=sizeof(cPacketHeader)+sizeof(cFragmentHeader); i<len; i+=4)
			fprintf(out, "%08X, ", *((DWORD *) (buf+i)));
	else
		for (int i=sizeof(cPacketHeader); i<len; i+=4)
			fprintf(out, "%08X, ", *((DWORD *) (buf+i)));
	fprintf(out, "\n");
	fclose(out);
#endif
	
	if ( pPacket->m_dwFlags == 0xA )
	{
		// This is an out-of-sequence error, means we fux0red up
		_ASSERTE ( FALSE );
		return 0;
	}
	
	if ( pPacket->m_nSequence > 0 )
		// Reset the client sequence
		m_dwClientSequence = pPacket->m_nSequence;
	
	switch ( m_state.m_stage )
	{
	case eStrobe:
		if ( pPacket->m_dwFlags == 0 )
			m_state.m_stage = eStrobe2;
		break;

	case eStrobe2:
		if ( pPacket->m_dwFlags == 0 )
		{
			// We received a version info - send part one and two to validate the current version
			postReceivePacket ( verinfo2, sizeof(verinfo2)); //102 + sizeof ( cPacketHeader ) );
//			for (int i=0;i<10;i++)
//				postReceivePacket ( stupidconnect, sizeof(stupidconnect));
		} else if ( pPacket->m_dwFlags == 0x80 ) {
//			postReceivePacket ( stupid2, sizeof(stupid2));
			m_state.m_stage = eVersionInfo;
		}
		break;
		
	case eVersionInfo:
		if ( pPacket->m_dwFlags == 0x4 )
		{
			m_state.m_stage = eNormalStream;
			m_dwServerSequence = 2;
			
			// Insert the MOTD/char list
			postFragment ( set_char_list_mode, 7, sizeof ( set_char_list_mode ) );
			::ResumeThread ( m_hTCPThread );

			sendFragment ( (void *) buf, len);
		}
		break;
		
	case eNormalStream:
		if (pPacket->m_dwFlags != 0x200)
		{
			sendFragment ( (void *) buf, len);
		}

		if ( pPacket->m_dwFlags == 0x4 )
			postPong ();
		else if ( pPacket->m_dwFlags == 0x20 )
			// This is a client disconnect
			m_state.m_stage = eDisconnected;
		else if ( pPacket->m_dwFlags == 0x200 )
		{
			// This is an app packet with a fragment
			const cFragmentHeader *pFragment = reinterpret_cast< const cFragmentHeader * > ( pPacket + 1 );
			BYTE *DTS = new BYTE[ pFragment->m_wLength - sizeof ( cFragmentHeader ) + sizeof(cPacketHeader) +4];
			memcpy(DTS, pPacket, sizeof(cPacketHeader)+4);
			memcpy(DTS+sizeof(cPacketHeader)+4, pFragment + 1, pFragment->m_wLength - sizeof(cFragmentHeader));
			sendFragment ( DTS, pFragment->m_wLength - sizeof ( cFragmentHeader ) + sizeof(cPacketHeader)+4 );
		}
		break;
		
	case eDisconnected:
		// Communicate no more!
		break;
	}
	
	return len;
}

int cUDPSocket::select ( fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout )
{
	_ASSERTE ( timeout != NULL );
	_ASSERTE ( exceptfds == NULL );
	
	DWORD dwTimeout = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
	DWORD dwWait = WAIT_TIMEOUT;
	
	if ( readfds != NULL )
	{
		_ASSERTE ( readfds->fd_count == 1 );
		_ASSERTE ( readfds->fd_array[ 0 ] == m_s );
		
		// Check if there's something to read
		dwWait = ::WSAWaitForMultipleEvents ( 1, &m_hInputWaiting, TRUE, dwTimeout, FALSE );
	}
	else if ( writefds != NULL )
	{
		_ASSERTE ( readfds->fd_count == 1 );
		_ASSERTE ( readfds->fd_array[ 0 ] == m_s );
		
		// We never block writes
		dwWait = WAIT_OBJECT_0;
	}
	
	return ( dwWait == WAIT_OBJECT_0 ) ? 1 : 0;
}

void cUDPSocket::postPacket ( cPacket &p )
{
	::EnterCriticalSection ( &m_cs );
	
	if ( m_input.empty () )
		::SetEvent ( m_hInputWaiting );
	
	m_input.push_back ( p );

#ifdef _DEBUG
	int len = p.m_wSize;
	BYTE *buf = p.m_pbPayload;
	FILE *out = fopen("c:\\clientlog.htm","ab");
	time_t tpt = time(NULL);
	struct tm *newtime = localtime(&tpt);
	char *tpstr = asctime(newtime);
	tpstr[24] = 0;
	DWORD flags = *((DWORD *) (buf+4)), seq = *((DWORD *) (buf+0));
	WORD nump = *((WORD *) (buf+0x1C)), curp = *((WORD *) (buf+0x20)), lenp = *((WORD *) (buf+0x1E));
	if (flags == 0x200)
		fprintf(out, "%s: In -> Seq:%i Flg:%08X Num:%i Cur:%i Len:%i - Data:", tpstr, seq, flags, nump, curp, lenp);
	else
		fprintf(out, "%s: In -> Seq:%i Flg:%08X - Data:", tpstr, seq, flags);
	if (flags == 0x200)
		for (int i=sizeof(cPacketHeader)+sizeof(cFragmentHeader); i<len; i+=4)
			fprintf(out, "%08X, ", *((DWORD *) (buf+i)));
	else
		for (int i=sizeof(cPacketHeader); i<len; i+=4)
			fprintf(out, "%08X, ", *((DWORD *) (buf+i)));
	fprintf(out, "\n");
	fclose(out);
#endif
	
	::LeaveCriticalSection ( &m_cs );
}

void cUDPSocket::postAppPacket ( void *buffer, size_t bytecount )
{
	cPacket p;
	p.m_wSize = bytecount + sizeof ( cPacketHeader );
	p.m_pbPayload = new BYTE[ p.m_wSize ];
	
	cPacketHeader *pHeader = reinterpret_cast< cPacketHeader * > ( p.m_pbPayload );
	pHeader->m_nSequence = ++ m_dwServerSequence;
	pHeader->m_dwFlags = 0x200;
	pHeader->m_dwServer = 0x0000000B;
	pHeader->m_dwCRC = 0xABCDEF12;
	pHeader->m_wSize = bytecount;
	pHeader->m_bTime = 3;
	pHeader->m_bUnknown = 0;
	
	::memcpy ( pHeader + 1, buffer, bytecount );
	
	// We're all set now - insert it into the read queue
	postPacket ( p );
}

void cUDPSocket::readPackets ()
{
	for ( ;; )
	{
		::EnterCriticalSection ( &m_cs );
		
		if ( m_inputstate == -1 )
		{
			// Read in the next header
			// Increment the fragment sequence as we're filling out headers when we read the bodies

			int ret = ::recv ( m_s, reinterpret_cast< char * > ( &m_currentinput ), sizeof ( EmuHeader ), MSG_PEEK );
			if ( ret != sizeof ( EmuHeader ) )
			{
				// No header right now - wait for the next one
//				int bad = WSAGetLastError();
				::LeaveCriticalSection ( &m_cs );
				return;
			}
			::recv ( m_s, reinterpret_cast< char * > ( &m_currentinput ), sizeof ( EmuHeader ), 0 );
//			++ m_dwFragmentSequence;
			m_inputstate = 0;
		}
		else
		{
			// We're reading a fragment from this header
			static WORD fragment_size = 448;	//464
			WORD wFragmentCount = m_currentinput.payloadsize_ / fragment_size;
			
			if ( wFragmentCount == m_inputstate )
			{
				// This is a partial fragment
				WORD wSize = m_currentinput.payloadsize_ - ( wFragmentCount * fragment_size ),
					wBufferSize = wSize + sizeof ( cFragmentHeader );
				BYTE *buffer = reinterpret_cast< BYTE * > ( _alloca ( wBufferSize ) );
				int ret = ::recv ( m_s, reinterpret_cast< char * > ( buffer + sizeof ( cFragmentHeader ) ), wSize, MSG_PEEK );
				if ( ret != wSize )
				{
					// Nothing to read here - come back later
#ifdef _DEBUG
	FILE *out = fopen("c:\\clientlog.htm","ab");
	fprintf(out, "ewwy2 - %i\n", ret);
	fclose(out);
#endif
					::LeaveCriticalSection ( &m_cs );
					return;
				}
				::recv ( m_s, reinterpret_cast< char * > ( buffer + sizeof ( cFragmentHeader ) ), wSize, 0 );

				// Fill out the fragment header and post the packet
				cFragmentHeader *pFrag = reinterpret_cast< cFragmentHeader * > ( buffer );
				pFrag->m_dwChannel = 0x80000000;
				pFrag->m_dwSequence = m_dwFragmentSequence;
				pFrag->m_wFragmentCount = wFragmentCount + 1;
				pFrag->m_wFragmentIndex = m_inputstate;
				pFrag->m_wLength = wBufferSize;
				pFrag->m_wUnknown = m_currentinput.message_;
				
				postAppPacket ( buffer, wBufferSize );
				
				// Reset the input state to read a new message
				m_inputstate = -1;
				m_dwFragmentSequence++;
			}
			else
			{
				static WORD buffer_size = fragment_size + sizeof ( cFragmentHeader );
				BYTE *buffer = reinterpret_cast< BYTE * > ( _alloca ( buffer_size ) );
				int ret = ::recv ( m_s, reinterpret_cast< char * > ( buffer + sizeof ( cFragmentHeader ) ), fragment_size, MSG_PEEK );
				if ( ret != fragment_size )
				{
					// Nothing to read here - come back later
#ifdef _DEBUG
	FILE *out = fopen("c:\\clientlog.htm","ab");
	fprintf(out, "ewwy3 - %i\n", ret);
	fclose(out);
#endif
					::LeaveCriticalSection ( &m_cs );
					return;
				}
				::recv ( m_s, reinterpret_cast< char * > ( buffer + sizeof ( cFragmentHeader ) ), fragment_size, 0 );

				// Fill out the fragment header and post the packet
				cFragmentHeader *pFrag = reinterpret_cast< cFragmentHeader * > ( buffer );
				pFrag->m_dwChannel = 0x80000000;
				pFrag->m_dwSequence = m_dwFragmentSequence;
				pFrag->m_wFragmentCount = wFragmentCount + 1;
				pFrag->m_wFragmentIndex = m_inputstate;
				pFrag->m_wLength = buffer_size;
				pFrag->m_wUnknown = m_currentinput.message_;
				
				postAppPacket ( buffer, buffer_size );
				
				// Advance to the next fragment
				++ m_inputstate;
			}
		}
		
		::LeaveCriticalSection ( &m_cs );
	}
}

bool cUDPSocket::writePackets ()
{
	// Write packets to the socket until there are no more to write
	// - or it fails, whichever comes first
	for ( ;; )
	{
		::EnterCriticalSection ( &m_cs );
		
		if ( m_output.empty () )
		{
			// We're done writing things out - the output queue is clear
			::WSAResetEvent ( m_hOutputWaiting );
			::LeaveCriticalSection ( &m_cs );
			break;
		}
		
		if ( m_outputstate == -1 )
		{
			//Write out the header for this packet
			EmuHeader h = { EmuHeader::Payload, 0, m_output.front ().m_wSize, m_dwSession };
			
			if ( ::send ( m_s, reinterpret_cast< char * > ( &h ), sizeof ( EmuHeader ), 0 ) != sizeof ( EmuHeader ) )
			{
				::LeaveCriticalSection ( &m_cs );
				return false;
			}
			
			m_outputstate = 0;
		}
		else
		{
			// Send a partition of the packet
			static WORD fragment_size = 448;
			WORD fragment_count = m_output.front ().m_wSize / fragment_size;
			BYTE *start = m_output.front ().m_pbPayload + fragment_size * fragment_count;
			
			if ( fragment_count == m_outputstate )
			{
				// This is a partial fragment - attempt to send it
				WORD wPartial = m_output.front ().m_wSize - fragment_size * fragment_count;
				
				if ( ::send ( m_s, reinterpret_cast< char * > ( start ), wPartial, 0 ) != wPartial )
				{
					::LeaveCriticalSection ( &m_cs );
					return false;
				}
				
				// Successfully sent the last remaining fragment - drop it from the list
				m_outputstate = -1;
				delete m_output.front ().m_pbPayload;
				m_output.pop_front ();
			}
			else
			{
				if ( ::send ( m_s, reinterpret_cast< char * > ( start ), fragment_size, 0 ) != fragment_size )
				{
					::LeaveCriticalSection ( &m_cs );
					return false;
				}
				
				// Sent this fragment of the message - set up for the next one
				++ m_outputstate;
			}
		}
		
		::LeaveCriticalSection ( &m_cs );
	}
	
	return true;
}

void cUDPSocket::threadRuntime ()
{
	m_inputstate = -1;
	m_outputstate = -1;
	
	WSAEVENT hSocketReady = ::WSACreateEvent ();
	int re = ::WSAEventSelect ( m_s, hSocketReady, FD_WRITE | FD_READ | FD_CLOSE );
	
	WSAEVENT hEvents[] = { m_hShutdown, hSocketReady, m_hOutputWaiting };
	int nWaits = 3;
	
	for ( ;; )
	{
		DWORD dwWait = ::WSAWaitForMultipleEvents ( nWaits, hEvents, FALSE, INFINITE, FALSE );
		switch ( dwWait )
		{
		case WSA_WAIT_EVENT_0:
			// We're shutting down - abort promptly
			return;
			
		case WSA_WAIT_EVENT_0 + 1:
			{
				// Some sort of socket transaction is ready to go
				WSANETWORKEVENTS wsan;
				::WSAEnumNetworkEvents ( m_s, hSocketReady, &wsan );

				if ( wsan.lNetworkEvents & FD_CLOSE )
					// We're done - the server dropped up
					return;
				
				if ( wsan.lNetworkEvents & FD_READ )
					readPackets ();
				
				if ( wsan.lNetworkEvents & FD_WRITE )
					nWaits = ( writePackets () ) ? 3 : 2;
				
				::WSAResetEvent ( hSocketReady );
			}
			break;
			
		case WSA_WAIT_EVENT_0 + 2:
			// We have packets to write
			nWaits = ( writePackets () ) ? 3 : 2;
			break;
		}
	}
}

DWORD WINAPI cUDPSocket::tcpThread ( LPVOID pvParam )
{
	cUDPSocket *pSocket = reinterpret_cast< cUDPSocket * > ( pvParam );
	pSocket->threadRuntime ();
	
	return 0;
}
