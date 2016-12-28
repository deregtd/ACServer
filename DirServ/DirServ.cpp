// DirServ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct stServerInfo {
	char Name[64];
	char IP[64];
	unsigned short Port;
	time_t LastUpdate;
	int Users;
	int Type;
	char Speed[16];
	char Admin[64];
};

int main()
{
	char buff[10000]; memset(buff, 0, 10000);
	long contentlength = 0;

	bool Quitting = false;
	
	printf("Content-type: text/html\n\n");

	char *omg = getenv("QUERY_STRING");
	if (omg)
	{
		contentlength = strlen(omg);
		strcpy(buff, omg);
	}
	
	char Keys[32][200], *posat = buff; int NumKeys = 0;
	memset(Keys, 0, sizeof(Keys));

	if (contentlength)
	{
		while (strchr(posat, '&'))
		{
			memcpy(Keys[NumKeys], posat, strchr(posat, '&') - posat);

			NumKeys++;

			posat = strchr(posat, '&') + 1;
		}
		
 		memcpy(Keys[NumKeys], posat, strlen(posat));
		NumKeys++;
	}

	int i;
	stServerInfo Server[256]; int NumServers = 0;

	time_t CurTime = time(NULL);

	FILE *in = fopen("serverlist.txt", "rb");
	if (!in)
		in = fopen("serverlist.txt", "wb");

	fseek(in, 0, SEEK_END);
	int flen = ftell(in);
	fseek(in, 0, SEEK_SET);

	NumServers = (int) (flen / sizeof(stServerInfo));
	if (NumServers > 256)
		NumServers = 256;
	fread(Server, NumServers * sizeof(stServerInfo), 1, in);

	fclose(in);

	char KeyLeft[200], KeyRight[200];
	
	bool MiniList = false, Serving = false;;
	
	bool NameSet = false, PortSet = false, TypeSet = false, AdminSet = false, SpeedSet = false, HostSet = false;
	Server[NumServers].Users = 0;
	for (i=0;i<NumKeys;i++)
	{
		memset(KeyLeft, 0, 200); memset(KeyRight, 0, 200);
		if (strchr(Keys[i], '='))
		{
			memcpy(KeyLeft, Keys[i], strchr(Keys[i], '=') - Keys[i]);
			memcpy(KeyRight, strchr(Keys[i], '=') + 1, strlen(strchr(Keys[i], '=') + 1));
		}

//		printf("Pair: %s = %s<BR>", KeyLeft, KeyRight);

		if (stricmp(KeyLeft, "Name") == 0) {
			strcpy(Server[NumServers].Name, KeyRight);
			while (strstr(Server[NumServers].Name, "%20"))
			{
				char *tpi = strstr(Server[NumServers].Name, "%20");
				*tpi = ' ';
				int tpl = strlen(tpi+3);
				memcpy(tpi+1, tpi+3, tpl);
				*(tpi + 1 + tpl) = 0;
			}
			NameSet = true;
		}
		if (stricmp(KeyLeft, "Admin") == 0) {
			strcpy(Server[NumServers].Admin, KeyRight);
			while (strstr(Server[NumServers].Admin, "%20"))
			{
				char *tpi = strstr(Server[NumServers].Admin, "%20");
				*tpi = ' ';
				int tpl = strlen(tpi+3);
				memcpy(tpi+1, tpi+3, tpl);
				*(tpi + 1 + tpl) = 0;
			}
			AdminSet = true;
		}
		if (stricmp(KeyLeft, "Speed") == 0) {
			strcpy(Server[NumServers].Speed, KeyRight);
			while (strstr(Server[NumServers].Speed, "%20"))
			{
				char *tpi = strstr(Server[NumServers].Speed, "%20");
				*tpi = ' ';
				int tpl = strlen(tpi+3);
				memcpy(tpi+1, tpi+3, tpl);
				*(tpi + 1 + tpl) = 0;
			}
			SpeedSet = true;
		}
		if (stricmp(KeyLeft, "Host") == 0) { strcpy(Server[NumServers].IP, KeyRight); HostSet = true; }
		if (stricmp(KeyLeft, "Port") == 0) { sscanf(KeyRight, "%i", &Server[NumServers].Port); PortSet = true; }
		if (stricmp(KeyLeft, "Type") == 0) { sscanf(KeyRight, "%i", &Server[NumServers].Type); TypeSet = true; }
		if (stricmp(KeyLeft, "Users") == 0) { sscanf(KeyRight, "%i", &Server[NumServers].Users); }
		if (stricmp(KeyLeft, "Quit") == 0) { Quitting = true; }
		if (stricmp(KeyLeft, "Minilist") == 0) { MiniList = true; }
	}
	
	if (PortSet && NameSet && TypeSet && SpeedSet && AdminSet)
	{
		if (!HostSet)
		{
			memset(Server[NumServers].IP, 0, 64);
			strcpy(Server[NumServers].IP, getenv("REMOTE_ADDR"));
		}
		
		Server[NumServers].LastUpdate = time(NULL);

		bool SrvF = false;
		for (i=0;i<NumServers;i++)
		{
			if ((stricmp(Server[i].IP, Server[NumServers].IP) == 0) && (Server[i].Port == Server[NumServers].Port))
			{
				memcpy(&Server[i], &Server[NumServers], sizeof(stServerInfo));
				i = NumServers;
				SrvF = true;
			}
		}
		
		if (!SrvF)
			NumServers++;

		Serving = true;
	}

	if (Quitting)
	{
		if (!HostSet)
		{
			memset(Server[NumServers].IP, 0, 32);

			char *tph = getenv("REMOTE_HOST");
			if (tph)
				strcpy(Server[NumServers].IP, tph);
			if (strlen(Server[NumServers].IP) == 0)
				strcpy(Server[NumServers].IP, getenv("REMOTE_ADDR"));
		}

		bool SrvF = false;
		for (i=0;i<NumServers;i++)
		{
			if ((stricmp(Server[i].IP, Server[NumServers].IP) == 0) && (Server[i].Port == Server[NumServers].Port))
				Server[i].Port = 0;
		}
	}

	FILE *out = fopen("serverlist.txt", "wb");
	if (MiniList)
	{
		printf("<TABLE BORDER=0 CELLSPACING=0 CELLPADDING=5 BORDERCOLOR=BLUE>\n");
		printf("<TR><TD WIDTH=130 ALIGN=CENTER><U>Server List</U></TD></TR>\n");
		for (i=0;i<NumServers;i++)
		{
//			if ((CurTime - Server[i].LastUpdate) > 60)
//				Server[i].Port = 0;

			if (Server[i].Port > 0)
			{
				printf("<TR><TD WIDTH=130 ALIGN=CENTER>");

				char shortname[21]; memset(shortname, 0, 21);
				_snprintf(shortname, 20, "%s", Server[i].Name);

				char mult[2] = { 0, 0 };
				if (Server[i].Users != 1)
					mult[0] = 's';

				printf("<FONT SIZE=2><A HREF=\"acserver://%s:%i/\">%.30s</A><BR>%s - %i User%s\n",
					Server[i].IP, Server[i].Port, Server[i].Name, Server[i].Speed, Server[i].Users, mult);
				
				printf("</TD></TR>");

				fwrite(&Server[i], sizeof(stServerInfo), 1, out);
			}
		}

		printf("</TABLE>\n");
	} else {
		printf("<HTML><HEAD><TITLE>Test Server List</TITLE></HEAD>\n");
		printf("<META HTTP-EQUIV=Refresh CONTENT=""15; URL=dirserv.exe"">\n");
		printf("<BODY BGCOLOR=BLACK TEXT=WHITE LINK=LIGHTBLUE>\n");

		printf("<TABLE BORDER=1 CELLSPACING=0 CELLPADDING=3 BORDERCOLOR=BLUE><TR><TH WIDTH=50></TH><TH>Server Name</TH><TH>Address:Port</TH><TH>Users</TH><TH>Speed</TH><TH>Admin</TH><TH>TSU</TH></TR>\n");

		for (i=0;i<NumServers;i++)
		{
			if ((CurTime - Server[i].LastUpdate) > 60)
				Server[i].Port = 0;

			if (Server[i].Port > 0)
			{
				printf("<TR><TD>");

				if (Server[i].Type == 1)
				{
					//Akilla
					printf("<A BORDER=0 HREF=""acserver://%s:%i/"">Connect</A>", Server[i].IP, Server[i].Port);
				} else if (Server[i].Type == 2) {
					//Sentou
					printf("<A HREF=""ace://%s:%i/"">Connect (Sentou)</A>", Server[i].IP, Server[i].Port);
				}

				char TSUColor[50];
				int TSU = CurTime - Server[i].LastUpdate;
				if (TSU < 20)
					sprintf(TSUColor, "GREEN");
				else if (TSU < 30)
					sprintf(TSUColor, "YELLOW");
				else
					sprintf(TSUColor, "RED");

				printf("</TD><TD>%s</TD><TD>%s:%i</TD><TD ALIGN=CENTER>%i/128</TD><TD ALIGN=CENTER>%s</TD><TD ALIGN=CENTER>%s</TD><TD ALIGN=CENTER><FONT COLOR=%s>%i</FONT></TD></TR>\n", Server[i].Name, Server[i].IP, Server[i].Port, Server[i].Users, Server[i].Speed, Server[i].Admin, TSUColor, TSU);

				fwrite(&Server[i], sizeof(stServerInfo), 1, out);
			}
		}

		printf("</TABLE><BR>\n");
		printf("<FONT SIZE=2><B>TSU</B> - Time Since Update.  Servers should update the page every 15 seconds.  If they lag out or have a problem, when TSU hits 60 seconds, the server is removed from the list.</FONT><P>\n");
		printf("<FONT SIZE=2>Hosted by Jubilee at <A HREF=http://dirserv.forotech.com/>http://dirserv.forotech.com/</A></FONT><BR>\n");
		printf("<FONT SIZE=2>This page refreshes every 15 seconds.  It works nicely as an active desktop item.</FONT>\n");
		printf("</BODY></HTML>\n");
	}

	fclose(out);

	return 0;
}

