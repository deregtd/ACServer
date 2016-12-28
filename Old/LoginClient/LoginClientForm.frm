VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form LoginForm 
   BackColor       =   &H00000000&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "ACSE Login Client"
   ClientHeight    =   2685
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6180
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2685
   ScaleWidth      =   6180
   StartUpPosition =   3  'Windows Default
   Begin MSWinsockLib.Winsock Winsock1 
      Left            =   0
      Top             =   0
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      Protocol        =   1
      LocalPort       =   9857
   End
   Begin VB.TextBox ServerName 
      Height          =   285
      Left            =   1320
      MaxLength       =   21
      TabIndex        =   7
      Text            =   "127.0.0.1:9002"
      Top             =   2220
      Width           =   1935
   End
   Begin VB.TextBox LoginPassword 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   3240
      MaxLength       =   20
      TabIndex        =   5
      Top             =   1200
      Width           =   2535
   End
   Begin VB.TextBox LoginName 
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   12
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   3240
      MaxLength       =   20
      TabIndex        =   3
      Top             =   480
      Width           =   2535
   End
   Begin VB.CommandButton SendPassword 
      Caption         =   "Send"
      Height          =   375
      Left            =   3600
      TabIndex        =   1
      Top             =   2160
      Width           =   1095
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   375
      Left            =   4920
      TabIndex        =   0
      Top             =   2160
      Width           =   1095
   End
   Begin VB.Label Label3 
      BackStyle       =   0  'Transparent
      Caption         =   "Server IP:Port:"
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   2280
      Width           =   1215
   End
   Begin VB.Label Label2 
      BackColor       =   &H00000000&
      BackStyle       =   0  'Transparent
      Caption         =   "Password:"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   24
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   615
      Left            =   360
      TabIndex        =   4
      Top             =   1080
      Width           =   2775
   End
   Begin VB.Label Label1 
      BackColor       =   &H00000000&
      BackStyle       =   0  'Transparent
      Caption         =   "Login Name:"
      BeginProperty Font 
         Name            =   "Tahoma"
         Size            =   24
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   615
      Left            =   360
      TabIndex        =   2
      Top             =   360
      Width           =   2775
   End
End
Attribute VB_Name = "LoginForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Base 0
Option Explicit

Private Sub Close_Click()
End
End Sub

Private Sub Form_Load()

'Check Registry for Login/Server
ServerName.Text = GetSetting("ACSE Login Client", "Settings", "Server")
LoginName.Text = GetSetting("ACSE Login Client", "Settings", "LoginName")
If ServerName.Text = "" Then ServerName.Text = "127.0.0.1:9002"

End Sub

Private Sub SendPassword_Click()
On Error GoTo lala

SaveSetting "ACSE Login Client", "Settings", "Server", ServerName.Text
SaveSetting "ACSE Login Client", "Settings", "LoginName", LoginName.Text

Dim Login(20) As Byte, Password(20) As Byte
Dim SendString(&H18& + 40) As Byte

Dim tpl As String, i As Integer

For i = 0 To &H18& + 40 - 1
    SendString(i) = 0
Next i

For i = 0 To 19
    Login(i) = 0
    Password(i) = 0
Next i

For i = 0 To Len(LoginName.Text) - 1
    Login(i) = Asc(Mid(LoginName.Text, i + 1, 1))
Next i
For i = 0 To Len(LoginPassword.Text) - 1
    Password(i) = Asc(Mid(LoginPassword.Text, i + 1, 1))
Next i

SendString(4) = &H55&
SendString(5) = &H34&
SendString(6) = &H1&
SendString(7) = &H0&

For i = 0 To 19
    SendString(&H18& + i) = Login(i)
    SendString(&H18& + 20 + i) = Password(i)
Next i

'Okay, packet is set up... Time to send...

Dim Server As String, Port As Integer

Server = ServerName.Text

For i = 0 To Len(Server) - 1
    If Mid(Server, i + 1, 1) = ":" Then
        Port = Val(Right(Server, Len(Server) - i - 1))
        Server = Left(Server, i)
        i = Len(Server)
    End If
Next i

Winsock1.Close
Winsock1.RemoteHost = Server
Winsock1.RemotePort = Port

Winsock1.SendData SendString
Winsock1.Close

Exit Sub

lala:
MsgBox "Error: " + Err.Source + " - " + Err.Description + " - " + Str(Err.LastDllError)

End Sub
