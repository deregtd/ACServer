// ClientAdapter.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ClientAdapter.h"

#include "ApiHook.h"
#include "Socket.h"

static cUDPSocket *m_pSocket = NULL;

// Functions for hooking from wsock32.dll
int PASCAL bindF ( SOCKET s, const struct sockaddr FAR *name, int namelen ); // 2
int PASCAL closesocketF ( SOCKET s ); // 3
int PASCAL getsockoptF ( SOCKET s, int level, int optname, char FAR *optval, int FAR *optlen ); // 7
int PASCAL ioctlsocketF ( SOCKET s, long cmd, u_long FAR *argp ); // 12
int PASCAL recvfromF ( SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen ); // 17
int PASCAL selectF ( int nfds, fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout ); // 18
int PASCAL sendtoF ( SOCKET s, const char FAR *buf, int len, int flags, const struct sockaddr FAR *to, int tolen ); // 20
int PASCAL setsockoptF ( SOCKET s, int level, int optname, const char FAR *optval, int optlen ); // 21
SOCKET PASCAL socketF ( int af, int type, int protocol ); // 23
int PASCAL WSAStartupF ( WORD wVersionRequested, LPWSADATA lpWSAData ); // 115
int PASCAL WSACleanupF (); // 116

static cHookDescriptor _hooks[] = {
   { eByOrdinal, _T( "wsock32.dll" ), _T( "bind" ), 2, reinterpret_cast< DWORD > ( bindF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "closesocket" ), 3, reinterpret_cast< DWORD > ( closesocketF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "getsockopt" ), 7, reinterpret_cast< DWORD > ( getsockoptF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "ioctlsocket" ), 12, reinterpret_cast< DWORD > ( ioctlsocketF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "recvfrom" ), 17, reinterpret_cast< DWORD >( recvfromF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "select" ), 18, reinterpret_cast< DWORD > ( selectF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "sendto" ), 20, reinterpret_cast< DWORD > ( sendtoF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "setsockopt" ), 21, reinterpret_cast< DWORD > ( setsockoptF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "socket" ), 23, reinterpret_cast< DWORD > ( socketF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "WSAStartup" ), 115, reinterpret_cast< DWORD > ( WSAStartupF ), 0 },
   { eByOrdinal, _T( "wsock32.dll" ), _T( "WSACleanup" ), 116, reinterpret_cast< DWORD > ( WSACleanupF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "bind" ), 2, reinterpret_cast< DWORD > ( bindF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "closesocket" ), 3, reinterpret_cast< DWORD > ( closesocketF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "getsockopt" ), 7, reinterpret_cast< DWORD > ( getsockoptF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "ioctlsocket" ), 12, reinterpret_cast< DWORD > ( ioctlsocketF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "recvfrom" ), 17, reinterpret_cast< DWORD >( recvfromF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "select" ), 18, reinterpret_cast< DWORD > ( selectF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "sendto" ), 20, reinterpret_cast< DWORD > ( sendtoF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "setsockopt" ), 21, reinterpret_cast< DWORD > ( setsockoptF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "socket" ), 23, reinterpret_cast< DWORD > ( socketF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "WSAStartup" ), 115, reinterpret_cast< DWORD > ( WSAStartupF ), 0 },
   { eByOrdinal, _T( "fsock32.dll" ), _T( "WSACleanup" ), 116, reinterpret_cast< DWORD > ( WSACleanupF ), 0 },
}; // 22

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
   if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
      DisableThreadLibraryCalls ( hModule );

   return TRUE;
}

static DWORD g_dwIP;
static std::string g_strUsername;
static std::string g_strPassword;
static short g_sConnPort;

// This is an example of an exported function.
extern "C" CLIENTADAPTER_API void InitClientAdapter ( DWORD dwIP, char *username, char *password, short ConnPort )
{
   in_addr addr;
   addr.S_un.S_addr = dwIP;

   TCHAR szMessage[ 255 ];
   ::_stprintf ( szMessage, _T( "Initialize Client Adapter: IP=%s, username=%s, password=%s" ),
      inet_ntoa ( addr ), username, password );

   g_dwIP = dwIP;
   g_strUsername = username;
   g_strPassword = password;
   g_sConnPort = ConnPort;

   DWORD dwPrevProtect;

/*   DWORD PatchLoc = 0x475A54, PatchSize=2;

   for (DWORD i=PatchLoc;i<PatchLoc+PatchSize;i+=2)
   {
	   WORD *pwTest = reinterpret_cast< WORD * > ( i );
	   ::VirtualProtect ( pwTest, sizeof ( WORD ), PAGE_EXECUTE_READWRITE, &dwPrevProtect );
	   *pwTest = 0x9090;
   }*/

   hookFunctions ( _hooks, 22, true );
}

int PASCAL bindF ( SOCKET s, const struct sockaddr FAR *name, int namelen )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   // We don't bind to a local socket - just connect
   return 0;
}

int PASCAL closesocketF ( SOCKET s )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   delete m_pSocket;
   m_pSocket = NULL;

   return 0;
}

int PASCAL getsockoptF ( SOCKET s, int level, int optname, char FAR *optval, int FAR *optlen )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   return getsockopt ( s, level, optname, optval, optlen );
}

int PASCAL ioctlsocketF ( SOCKET s, long cmd, u_long FAR *argp )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   static DWORD commands[] = {
      FIONREAD,
      FIONBIO,
      FIOASYNC,
      SIOCSHIWAT,
      SIOCGHIWAT,
      SIOCSLOWAT,
      SIOCGLOWAT,
      SIOCATMARK, };

   return ioctlsocket ( s, cmd, argp );
}

int PASCAL recvfromF ( SOCKET s, char FAR* buf, int len, int flags, struct sockaddr FAR* from, int FAR* fromlen )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   return m_pSocket->recvfrom ( buf, len, flags, from, fromlen );
}

int PASCAL selectF ( int nfds, fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout )
{
   _ASSERTE ( m_pSocket != NULL );

   return m_pSocket->select ( readfds, writefds, exceptfds, timeout );
}

int PASCAL sendtoF ( SOCKET s, const char FAR *buf, int len, int flags, const struct sockaddr FAR *to, int tolen )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   return m_pSocket->sendto ( buf, len, flags, to, tolen );
}

int PASCAL setsockoptF ( SOCKET s, int level, int optname, const char FAR *optval, int optlen )
{
   _ASSERTE ( m_pSocket != NULL );
   _ASSERTE ( m_pSocket->getSocket () == s );

   return setsockopt ( s, level, optname, optval, optlen );
}

SOCKET PASCAL socketF ( int af, int type, int protocol )
{
   _ASSERTE ( m_pSocket == NULL );
   m_pSocket = new cUDPSocket ( g_dwIP, g_strUsername.c_str (), g_strPassword.c_str (), g_sConnPort );

   _ASSERTE ( m_pSocket->getSocket () != 0 );

   return m_pSocket->getSocket ();
}

int PASCAL WSAStartupF ( WORD wVersionRequested, LPWSADATA lpWSAData )
{
   _ASSERTE ( m_pSocket == NULL );

   return WSAStartup ( wVersionRequested, lpWSAData );
}

int PASCAL WSACleanupF ()
{
   _ASSERTE ( m_pSocket == NULL );

   return WSACleanup ();
}
