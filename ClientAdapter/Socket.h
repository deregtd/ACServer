// Socket.h
// Delcaration of class UDP Socket - spoof for the UDP protocol

#pragma once

#include "PacketHeaders.h"

class cUDPSocket
{
   CRITICAL_SECTION m_cs;
   WSAEVENT m_hInputWaiting,
      m_hOutputWaiting,
      m_hShutdown;
   HANDLE m_hTCPThread;

   struct cPacket
   {
      BYTE *m_pbPayload;
      WORD m_wSize;
   };

   typedef std::list< cPacket > cPacketQueue;
   cPacketQueue m_input,
      m_output;

   EmuHeader m_currentinput;

   int m_inputstate,
      m_outputstate;

   sockaddr_in m_connect;
   SOCKET m_s;

   enum eConnectStage
   {
      eStrobe,
      eStrobe2,
      eVersionInfo,
      eNormalStream,
      eDisconnected
   };

   struct state_bits
   {
      bool m_bConnected : 1;
      eConnectStage m_stage : 3;
   };

   state_bits m_state;

   int m_dwServerSequence,
      m_dwClientSequence,
      m_dwFragmentSequence;

   DWORD m_dwSession;

public:
   cUDPSocket ( DWORD dwIP, const char *szUserName, const char *szPassword, short ConnPort );
   ~cUDPSocket ();

   inline SOCKET getSocket ()
   {
      return m_s;
   }

   int recvfrom ( char *buf, int len, int flags, sockaddr FAR *from, int *fromlen );
   int sendto ( const char FAR *buf, int len, int flags, const struct sockaddr FAR *to, int tolen );
   int select ( fd_set FAR *readfds, fd_set FAR *writefds, fd_set FAR *exceptfds, const struct timeval FAR *timeout );

private:
   void postPacket ( cPacket &p );
   void postReceivePacket ( void *buf, int len );
   void postAppPacket ( void *buffer, size_t bytecount );
   void postPong ();
   void postFragment ( void *buffer, WORD wClass, size_t bytecount );

   void sendFragment ( void *payload, size_t bytecount );

#pragma pack(push,1)
   struct cPacketHeader
   {
      int m_nSequence;
      DWORD m_dwFlags;
      DWORD m_dwCRC;
      DWORD m_dwServer;
      WORD m_wSize;
      BYTE m_bTime;
      BYTE m_bUnknown;
   };

   struct cFragmentHeader
   {
      DWORD m_dwSequence,
         m_dwChannel;
      WORD m_wFragmentCount,
         m_wLength,
         m_wFragmentIndex,
         m_wUnknown;
   };
#pragma pack(pop)

   void readPackets ();
   bool writePackets ();
   void threadRuntime ();
   static DWORD WINAPI tcpThread ( LPVOID pvParam );
};
