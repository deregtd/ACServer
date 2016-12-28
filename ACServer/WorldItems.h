#ifndef WORLDITEMS_H
#define WORLDITEMS_H

#define OBJECT_CHARACTER				1
#define OBJECT_LIFESTONE				2
#define OBJECT_PORTAL					3
#define OBJECT_WEAPON					4
#define OBJECT_OBJECTID					1000

#define PORTAL_PLAYERSUMMONED	1
#define PORTAL_TYABLE			2
#define PORTAL_SUMMONABLE		4
#define PORTAL_RECALLABLE		8

#define WEAPON_UA		1
#define WEAPON_DAGGER	2
#define WEAPON_STAFF	3
#define WEAPON_SWORD	4
#define WEAPON_SPEAR	5
#define WEAPON_MACE		6
#define WEAPON_AXE		7
#define WEAPON_THROWN	8
#define WEAPON_BOW		9
#define WEAPON_XBOW		10
#define WEAPON_POLEARM	11

#define SPOT_FTORSO		0
#define SPOT_BTORSO		1
#define SPOT_FGIRTH		2
#define SPOT_BGIRTH		3
#define SPOT_UARM		4
#define SPOT_LARM		5
#define SPOT_HAND		6
#define SPOT_ULEG		7
#define SPOT_LLEG		8
#define SPOT_FOOT		9
#define SPOT_EYE		10
#define SPOT_NOSE		11
#define SPOT_MOUTH		12

#define BYOBJ_SELECTABLE 1

#define SIZEOFCHAR		sizeof(cWorldObject_Char) - sizeof(cWorldObject)
//#define SIZEOFCHAR		32+8+4+(96*4)+8+(4*12)+4+12+(2*sizeof(Location_t))+4+(3*4*6)+(4*4*3)+(10*4)+(5*100*4)+4+(40*5*4)+(102*4)+(7*4)+(25*4)+8+4+(3*2*17)
//#define SIZEOFCHAR		32+8+4+(96*4)+8+(4*12)+4+12+(2*sizeof(Location_t))+4+(3*4*6)+(4*4*3)+(10*4)+(5*100*4)+4+(40*5*4)+(102*4)+(7*4)+(25*4)+8+4

void GenString(char *str2c, int *outlen, BYTE *outbuf);

typedef struct
{
	DWORD		landblock;	//turbine
	float		x,y,z;		//cartesian (relative to LB)
	float		a,b,c,w;	//quaternion
} Location_t;

class cWorldObject {
public:
	DWORD Type;
	DWORD Size;

	DWORD GUID;
	char Name[32];
	Location_t Loc;

	DWORD Owner;

	cWorldObject()
	{
		Size = 4+4+4+32+sizeof(Location_t);
	}
	~cWorldObject() { }
	virtual int Serialize(BYTE *Data)
	{
		Data = 0;
		return(0);
	}
	virtual void LoadFrom(BYTE *Here)
	{
	}
	virtual int GenerateCreatePacket(BYTE * Packet)
	{
		return(0);
	}
};

class cWorldObject_LS : public cWorldObject {
public:
	cWorldObject_LS() 
	{
		Size = 0;
		Type = OBJECT_LIFESTONE;
		Owner = 0;
	}

	virtual int Serialize(BYTE *Data)
	{
		Data = 0;
		return(0);
	}
	virtual void LoadFrom(BYTE *Here)
	{
	}

	virtual int GenerateCreatePacket(BYTE * Packet)
	{
/*		BYTE * PackPointer = Packet;
		DWORD tpd;	WORD tpw;
		
		//F7b0 header
		tpd = 0xF745;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
							memcpy(PackPointer, &GUID , 4);	PackPointer += 4;
		
		tpd = 11;			memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//11
		tpd = 0x0013;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//PaletteCount...
		tpd = 0x0013;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//TextureCount...
		tpd = 0x0013;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//ModelCount...

		return((DWORD) PackPointer - (DWORD) Packet);*/

		unsigned char CREATE_ENT_LS[] = {
			0x45, 0xF7, 0x00, 0x00, 0x01, 0xE0, 0xD9, 0x7B, 0x11, 0x00, 0x00, 0x00, 0x03, 0x88, 0x01, 0x00,
			0x10, 0x04, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0xF0, 0xD0, 0x03,
			0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x9E, 0xBD, 0x24, 0x68, 0x29, 0x42, 0x7B, 0xC3, 0x6A, 0x42,
			0xF0, 0x88, 0x19, 0x41, 0xD5, 0x28, 0xCC, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x73, 0xC4, 0x6A, 0xBF, 0x26, 0x00, 0x00, 0x09, 0x14, 0x00, 0x00, 0x20, 0xEE, 0x02, 0x00, 0x02, 
			0x44, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x04, 0x00, 0x00, 0x00, 0x38, 0x00, 0x80, 0x00, 0x0B, 0x00, 0x4C, 0x69, 0x66, 0x65, 0x20, 0x53, 
			0x74, 0x6F, 0x6E, 0x65, 0x00, 0x00, 0x00, 0x00, 0xFD, 0x01, 0x55, 0x13, 0x80, 0x00, 0x00, 0x00,
			0x14, 0x40, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 
			0x04, 0x00, 0x00, 0x00, 0x06, 0x56, 0xFA, 0x02
		};

		memcpy(&CREATE_ENT_LS[4], &GUID, 4);
		memcpy(&CREATE_ENT_LS[0x24], &Loc, sizeof(Loc));
		memcpy(Packet, CREATE_ENT_LS, sizeof(CREATE_ENT_LS));
		
		return(sizeof(CREATE_ENT_LS));
	}
};

class cWorldObject_Portal : public cWorldObject {
public:
	Location_t DestLoc;
	DWORD Flags;
	WORD Lower, Upper;
/*		1 = PlayerSummoned
		2 = Tyable
		4 = Summonable
		8 = Recallable
		00FF0000 = Lower Limit
		FF000000 = Upper Limit
		0000FFF0 = Expansion */

	cWorldObject_Portal() 
	{
		Size = sizeof(DestLoc) + sizeof(DWORD);
		Type = OBJECT_PORTAL;
		Flags = PORTAL_RECALLABLE | PORTAL_TYABLE | PORTAL_SUMMONABLE;
		Owner = 0;
	}

	virtual int Serialize(BYTE *Data)
	{
		Data = (BYTE *) &DestLoc;
		return(Size);
	}
	virtual void LoadFrom(BYTE *Here)
	{
		memcpy(&DestLoc, &Here[0], sizeof(DestLoc));
		memcpy(&Flags, &Here[sizeof(DestLoc)], sizeof(DWORD));
	}

	virtual int GenerateCreatePacket(BYTE * Packet)
	{
		unsigned char CREATE_ENT_PORTAL_A[] = {
			0x45, 0xF7, 0x00, 0x00, 0x37, 0xE0, 0x17, 0x70, 0x11, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0x00, 	//E˜..7‡.p.....Ä..
			0x0C, 0x0C, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x00, 0xF0, 0xD0, 0x03, 	//........=..–.
			0x00, 0x00, 0x00, 0x00, 0x6E, 0x01, 0x7E, 0x01, 0x00, 0x00, 0xF0, 0x41, 0x00, 0x00, 0x04, 0xC3, 	//....n.~...A...√
			0x20, 0x06, 0x81, 0xBD, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// .ÅΩ..Ä?........
			0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x09, 0xB3, 0x01, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 	//........≥.......
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA9, 0x00, 0x00, 0x00, 	//............©...
			0x38, 0x00, 0x80, 0x00,
		};
		unsigned char CREATE_ENT_PORTAL_B[] = {
			0x09, 0x12, 0x6B, 0x10, 0x80, 0x00, 0x01, 0x00, 0x14, 0x00, 0x04, 0x00, 	//l.....k.Ä.......
			0x80, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0xCD, 0xCC, 0xCC, 0xBD, 0x04, 0x00, 0x00, 0x00, 	//Ä... ...ÕÃÃΩ....
		};
		BYTE * PackPointer = Packet;
		memcpy(&CREATE_ENT_PORTAL_A[4], &GUID, 4);
		memcpy(&CREATE_ENT_PORTAL_A[0x24], &Loc, sizeof(Loc));
		memcpy(PackPointer, CREATE_ENT_PORTAL_A, sizeof(CREATE_ENT_PORTAL_A)); PackPointer += sizeof(CREATE_ENT_PORTAL_A);
		int outlen; GenString(Name, &outlen, PackPointer); PackPointer += outlen;
		memcpy(PackPointer, CREATE_ENT_PORTAL_B, sizeof(CREATE_ENT_PORTAL_B)); PackPointer += sizeof(CREATE_ENT_PORTAL_B);

		return((DWORD) PackPointer - (DWORD) Packet);
	}
};

class cWorldObject_Char : public cWorldObject {
public:
	char Title[32];

	DWORD TotalXP, UnassignedXP;
	DWORD SkillCredits;

	DWORD SpellsLearned[96];
	DWORD Patron, Monarch, Vassal[12];
	DWORD Options;
	
	DWORD Race, Gender, Class, Level;

	Location_t LSTie, PortalTie;
	WORD PortalTieMinLevel, PortalTieMaxLevel;
	
	DWORD InitialStat[6], CurrentStat[6], XPIntoStat[6];
	DWORD SecondaryStatInc[3], CurrentSecondaryStat[3], XPIntoSecondaryStat[3], BaseSecondaryStat[3];

	DWORD ShortCut[10];
	DWORD SpellBar[5][100];

	DWORD Vitae;

	DWORD SkillInc[40], SkillTrain[40], SkillXP[40], SkillFreeXP[40], SkillBase[40];
	
	DWORD MainPack[102];
	DWORD SidePacks[7];

	DWORD Equipped[25];	//Equipped GUIDs

	WORD Model, NumLogins, Owner, Expand;

	float Scale;
	
	WORD ModelSwap[17];
	WORD OldTexSwap[17], TexSwap[17];

	cWorldObject_Char() 
	{
		Size = SIZEOFCHAR;
		Type = OBJECT_CHARACTER;
		Owner = 0;
//		ZeroMemory(ModelSwap, 17*2);
//		ZeroMemory(TexSwap, 17*2);
	}

	virtual int Serialize(BYTE *Data)
	{
		memcpy(Data, Title, SIZEOFCHAR);
		return(SIZEOFCHAR);
	}
	virtual void LoadFrom(BYTE *Here)
	{
		memcpy(Title, Here, Size);
	}

	int GenerateTexChangePacket(BYTE *Packet)
	{
		BYTE * PackPointer = Packet;
		DWORD tpd;	WORD tpw;	BYTE tpb;

		tpd = 0xF625;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
							memcpy(PackPointer, &GUID, 4);	PackPointer += 4;
	
		tpb = 0x11;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//11
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//PaletteCount...
		BYTE MSwpCnt = 0;
		for (int i=0;i<17;i++)
		{
			if (TexSwap[i])
				MSwpCnt++;
		}
		tpb = MSwpCnt;		memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//TextureCount...
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//ModelCount...

		//texswaps
		for (i=0;i<17;i++)
		{
			if (TexSwap[i])
			{
				tpb = i;	memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//ID
				tpw = OldTexSwap[i];memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Swap
				tpw = TexSwap[i];	memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Swap
			}
		}
		tpw = 0;			memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//modelsequencetype
		tpw = 0;			memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//modelsequence

		return((DWORD) PackPointer - (DWORD) Packet);
	}

	virtual int GenerateCreatePacket(BYTE *Packet)
	{
		BYTE * PackPointer = Packet;
		DWORD tpd;	WORD tpw;	BYTE tpb;// int i;
		
		tpd = 0xF745;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
							memcpy(PackPointer, &GUID, 4);	PackPointer += 4;
	
//		tpb = 0x11;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//11
//		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//PaletteCount...
//		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//TextureCount...
//		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//ModelCount...

		//Header
		*((BYTE *)PackPointer)	= 0x11;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 3;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 4;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 17;		PackPointer += 1;

		//PaletteSwaps
		*((WORD *)PackPointer)	= 0x007E;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xB3;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 0x02;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x1800;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xE1;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 0x02;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0818;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xAF;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 0x04;		PackPointer += 1;
		//PaletteUnknown
		*((WORD *)PackPointer)	= 0x0820;	PackPointer += 2;

		//TextureSwaps
		*((BYTE *)PackPointer)	= 10;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0098;	PackPointer += 2;
		*((WORD *)PackPointer)	= 0x10B7;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 10;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x024C;	PackPointer += 2;
		*((WORD *)PackPointer)	= 0x1120;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 10;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x02F5;	PackPointer += 2;
		*((WORD *)PackPointer)	= 0x1162;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 10;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x025C;	PackPointer += 2;
		*((WORD *)PackPointer)	= 0x11EA;	PackPointer += 2;
		
		//ModelSwaps
		*((BYTE *)PackPointer)	= 0;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x004E;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 1;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x004F;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 2;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x004D;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 5;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0053;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 6;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0051;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 9;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0054;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xA;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0497;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xB;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0495;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xC;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0076;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xD;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x04AD;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xE;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0496;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0xF;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0077;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 3;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x004C;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 7;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0050;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 4;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x004B;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 8;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x0052;	PackPointer += 2;
		*((BYTE *)PackPointer)	= 0x10;		PackPointer += 1;
		*((WORD *)PackPointer)	= 0x049E;	PackPointer += 2;

		//Filler
		*((BYTE *)PackPointer)	= 0x00;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 0x00;		PackPointer += 1;
		*((BYTE *)PackPointer)	= 0x00;		PackPointer += 1;

		tpd = 0x000198C3;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags1
		tpd = 0x00404410;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown2

		//Flag1 Stuff Begin
		tpd = 0x00000008;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknownCount
		BYTE UnkBytes[8] = { 0x00, 0x00, 0x3D, 0x00, 0x00, 0xF0, 0xE7, 0x03 };
							memcpy(PackPointer, UnkBytes, 8);	PackPointer += 8;	//unknownBytes
		tpd = 0x00000000;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknownDword

							memcpy(PackPointer, &Loc, 32);	PackPointer += 32;		//location	-  8000
		tpd = 0x09000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Grey		-     2
		tpd = 0x20000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Yellow	-   800
		tpd = 0x34000004;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Blue		-  1000
		tpd = 0x02000000 | Model; memcpy(PackPointer, &tpd, 4);	PackPointer += 4;	//Skyblue	-     1		Model

		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Purple	-    40 - starts at 0xAC
		tpd = 0xB8548268;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Purple	-    40 - starts at 0xAC
		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Purple	-    40 - starts at 0xAC
							memcpy(PackPointer, &Scale, 4);	PackPointer += 4;		//Green		-    80		Scale
		//Flags1 Stuff End

		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[0]=wNum_Move_Interact;
		tpw = 0x0001;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[1]=wNum_Anim_Interact;
		tpw = 0x0001;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[2]=wNum_Bubble_Mode;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[3]=wNum_Jumps;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[4]=wNum_Portals; (Bubble_Mode in a sense maybe)
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[5]=wNum_Anim_Count;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[6]=wNum_Override;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[7]=
		tpw = 0x0003;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[8]=wNum_Logins;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[9]=

		tpd = 0x00800016;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags2

		int outlen;	GenString(Name, &outlen, PackPointer);	PackPointer += outlen;	//objectName

		tpw = 0x0001;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//"model"
		tpw = 0x1036;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Icon

		tpd = 0x00000010;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_2	 - Monster... lol
		
		tpd = 0x0000003C;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_3	 - NotGrabbed+Player+Selectable+PK

		//Flags2 Stuff Begin
		tpb = 0x66;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//itemSlots
		tpb = 0x07;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//packSlots
		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//total value
		tpd = 0x00000004;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v6
		//Flags2 Stuff End

		return((DWORD) PackPointer - (DWORD) Packet);
	}
};

class cWorldObject_ObjectID : public cWorldObject {
public:
	WORD Model, Icon;
	DWORD Flags;
	float Scale;

	cWorldObject_ObjectID() 
	{
		Size = sizeof(Model)*2 + sizeof(Flags) + sizeof(Scale);
		Type = OBJECT_OBJECTID;
		Owner = 0;
		Scale = 1.0f;
	}

	virtual int Serialize(BYTE *Data)
	{
		BYTE ForData[sizeof(Model)*2 + sizeof(Flags)];
		memcpy(&ForData[0], &Model, sizeof(Model));
		memcpy(&ForData[2], &Icon, sizeof(Icon));
		memcpy(&ForData[4], &Flags, sizeof(Flags));
		memcpy(&ForData[8], &Scale, sizeof(Scale));
		Data = (BYTE *) &ForData;
		return(sizeof(ForData));
	}
	virtual void LoadFrom(BYTE *Here)
	{
		memcpy(&Model, Here, sizeof(Model));
		memcpy(&Icon, Here+2, sizeof(Icon));
		memcpy(&Flags, Here + 4, sizeof(Flags));
		memcpy(&Scale, Here + 8, sizeof(Scale));
	}

	virtual int GenerateCreatePacket(BYTE * Packet)
	{
		BYTE * PackPointer = Packet;
		DWORD tpd;	WORD tpw;	BYTE tpb;
		
		tpd = 0xF745;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
							memcpy(PackPointer, &GUID, 4);	PackPointer += 4;
	
		tpb = 0x11;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//11
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//PaletteCount...
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//TextureCount...
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//ModelCount...

		tpd = 0x00029881;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags1
		tpd = 0x00000414;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown2

		//Flag1 Stuff Begin
		tpd = 0x00000065;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Unknown	- 20000
							memcpy(PackPointer, &Loc, 32);	PackPointer += 32;		//location	-  8000
		tpd = 0x20000014;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Yellow	-   800
		tpd = 0x3400002B;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Blue		-  1000
		tpd = 0x02000000 | Model; memcpy(PackPointer, &tpd, 4);	PackPointer += 4;	//Skyblue	-     1		Model
							memcpy(PackPointer, &Scale, 4);	PackPointer += 4;		//Green		-    80		Scale
		//Flags1 Stuff End

		tpw = 0x0004;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[0]=wNum_Move_Interact;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[1]=wNum_Anim_Interact;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[2]=wNum_Bubble_Mode;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[3]=wNum_Jumps;
		tpw = 0x0001;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[4]=wNum_Portals; (Bubble_Mode in a sense maybe)
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[5]=wNum_Anim_Count;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[6]=wNum_Override;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[7]=
		tpw = 0x0005;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[8]=wNum_Logins;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[9]=

		tpd = 0x10010218;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags2

		int outlen;	GenString(Name, &outlen, PackPointer);	PackPointer += outlen;	//objectName


		tpw = 0x020F;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//"model"
		tpw = 0x10CE;		memcpy(PackPointer, &Icon, 2);	PackPointer += 2;		//Icon

		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_2	 - Jewelry :)
		
		tpd = 0;
		if (Flags & BYOBJ_SELECTABLE)
			tpd |= 0x00000010;
		else
			tpd |= 0x00000080;
							memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_3	 - Cannot be selected

		//Flags2 Stuff Begin
		tpd = 0x0000000A;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//value
		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//total value
		tpd = 0x10000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//equipment type
		tpd = 0x00000200;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//coverage
		tpd = 0x00134C52;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown
		//Flags2 Stuff End

		return((DWORD) PackPointer - (DWORD) Packet);
	}
};

class cWorldObject_Weapon : public cWorldObject {
public:
	WORD Model, Icon;
	int WeapType;

	float DamageRatio, DamageMax;
	float AttackMod, DefenseMod;
	DWORD DamageType, Speed, Range, DamageMod;

	DWORD Spellcraft, Difficulty;
	DWORD RankReq, RaceReq;
	DWORD SkillReq, SkillReqAmount;
	DWORD Enchants[8];

	cWorldObject_Weapon() 
	{
		Size = sizeof(cWorldObject_Weapon);
		Type = OBJECT_WEAPON;
		Owner = 0;
	}

	virtual int Serialize(BYTE *Data)
	{
		BYTE ForData[sizeof(cWorldObject_Weapon)];
		memcpy(ForData, &Model, Size);
		Data = (BYTE *) &ForData;
		return(sizeof(ForData));
	}
	virtual void LoadFrom(BYTE *Here)
	{
		memcpy(&Model, Here, Size);
	}

	virtual int GenerateCreatePacket(BYTE * Packet)
	{
		BYTE * PackPointer = Packet;
		DWORD tpd;	WORD tpw;	BYTE tpb;
		
		tpd = 0xF745;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
							memcpy(PackPointer, &GUID, 4);	PackPointer += 4;
	
		tpb = 0x11;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//11
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//PaletteCount...
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//TextureCount...
		tpb = 0x00;			memcpy(PackPointer, &tpb, 1);	PackPointer += 1;		//ModelCount...

		tpd = 0x00029881;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags1
		tpd = 0x00000414;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown2

		//Flag1 Stuff Begin
		tpd = 0x00000065;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Unknown	- 20000
							memcpy(PackPointer, &Loc, 32);	PackPointer += 32;		//location	-  8000
		tpd = 0x20000014;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Yellow	-   800
		tpd = 0x3400002B;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Blue		-  1000
		tpd = 0x02000000 | Model; memcpy(PackPointer, &tpd, 4);	PackPointer += 4;	//Skyblue	-     1		Model
		tpd = 0x3F8A3D71;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Green		-    80
		//Flags1 Stuff End

		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[0]=wNum_Move_Interact;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[1]=wNum_Anim_Interact;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[2]=wNum_Bubble_Mode;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[3]=wNum_Jumps;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[4]=wNum_Portals; (Bubble_Mode in a sense maybe)
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[5]=wNum_Anim_Count;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[6]=wNum_Override;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[7]=
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[8]=wNum_Logins;
		tpw = 0x0000;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//Seagreen1 - vitals[9]=

		tpd = 0x10010218;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags2

		int outlen;	GenString(Name, &outlen, PackPointer);	PackPointer += outlen;	//objectName


		tpw = 0x020F;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//"model"
							memcpy(PackPointer, &Icon, 2);	PackPointer += 2;		//Icon

		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_2	 - Weapon
		
		tpd = 0x12;			memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown_v0_3	 - Cannot be selected

		//Flags2 Stuff Begin
		tpd = 0x0000000A;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//value
		tpd = 0x00000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//total value
		tpd = 0x10000001;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//equipment type
		tpd = 0x00000200;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//coverage
		tpd = 0x00134C52;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown
		//Flags2 Stuff End

		return((DWORD) PackPointer - (DWORD) Packet);
	}
};


#endif