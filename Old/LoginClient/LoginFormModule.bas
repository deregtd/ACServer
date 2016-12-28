Attribute VB_Name = "LoginFormModule"
Public Declare Sub CopyMemory Lib "KERNEL32" Alias "RtlMoveMemory" (Destination As Any, Source As Any, ByVal Length As Long)
Public Declare Sub ZeroMemory Lib "KERNEL32" Alias "RtlMoveMemory" (dest As Any, ByVal numBytes As Long)

