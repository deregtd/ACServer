// PacketHeaders.h
// Declaration of Packet realted structures common to both the client and server

#pragma once

#pragma pack(push,1)
struct EmuHeader
{
   enum PacketType { 
      // Client to Server: login/pass
      Authenticate,
      // Server to Client: assign a session ID
      SetSession,
      // Client to Server: rejoin session on new node
      JoinSession,
      // Both: Terminate nicely
      Disconnect,
      // Both: Game related events
      Payload };

   // Enum PacketType
   BYTE type_,
   // Unknown message flag (priority?)
      message_;
   // Bytes in the payload
   WORD payloadsize_;
   // Session ID
   DWORD session_;
};

struct EmuAuthenticate
{
   char username_[ 20 ],
      password_[ 20 ];
};

#pragma pack(pop)
