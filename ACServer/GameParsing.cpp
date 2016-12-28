#include "stdafx.h"
#include "GameParsing.h"

stSkillInfo SkillInfo[] = {
      { _T( "Unknown" ), STAT_NULL, STAT_NULL, 1, eTrainUnusable },
      { _T( "Axe" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Bow" ), STAT_COORDINATION, STAT_NULL, 2, eTrainUnusable },
      { _T( "Crossbow" ), STAT_COORDINATION, STAT_NULL, 2, eTrainUnusable },
      { _T( "Dagger" ), STAT_QUICKNESS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Mace" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Melee Defense" ), STAT_QUICKNESS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Missile Defense" ), STAT_QUICKNESS, STAT_COORDINATION, 5, eTrainUnusable },
      { _T( "Unknown" ), STAT_NULL, STAT_NULL, 1, eTrainUnusable },
      { _T( "Spear" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Staff" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Sword" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Thrown Weapons" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Unarmed Combat" ), STAT_STRENGTH, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Arcane Lore" ), STAT_FOCUS, STAT_NULL, 3, eTrainUnusable },
      { _T( "Magic Defense" ), STAT_FOCUS, STAT_SELF, 7, eTrainUnusable },
      { _T( "Mana Conversion" ), STAT_FOCUS, STAT_SELF, 6, eTrainUnusable },
      { _T( "Unknown" ), STAT_NULL, STAT_NULL, 1, eTrainUnusable },
      { _T( "Appraise Item" ), STAT_FOCUS, STAT_NULL, 1, eTrainUnusable },
      { _T( "Assess Person" ), STAT_FOCUS, STAT_SELF, 2, eTrainUnusable },
      { _T( "Deception" ), STAT_FOCUS, STAT_SELF, 4, eTrainUnusable },
      { _T( "Healing" ), STAT_FOCUS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Jump" ), STAT_STRENGTH, STAT_COORDINATION, 2, eTrainUnusable },
      { _T( "Lockpick" ), STAT_FOCUS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Run" ), STAT_QUICKNESS, STAT_NULL, 1, eTrainUnusable },
      { _T( "Unknown" ), STAT_NULL, STAT_NULL, 1, eTrainUnusable },
      { _T( "Unknown" ), STAT_NULL, STAT_NULL, 1, eTrainUnusable },
      { _T( "Assess Creature" ), STAT_FOCUS, STAT_SELF, 2, eTrainUnusable },
      { _T( "Appraise Weapon" ), STAT_FOCUS, STAT_NULL, 1, eTrainUnusable },
      { _T( "Appraise Armor" ), STAT_FOCUS, STAT_NULL, 1, eTrainUnusable },
      { _T( "Appraise Magic Item" ), STAT_FOCUS, STAT_NULL, 1, eTrainUnusable },
      { _T( "Creature Enchantment" ), STAT_FOCUS, STAT_SELF, 4, eTrainUnusable },
      { _T( "Item Enchantment" ), STAT_FOCUS, STAT_SELF, 4, eTrainUnusable },
      { _T( "Life Magic" ), STAT_FOCUS, STAT_SELF, 4, eTrainUnusable },
      { _T( "War Magic" ), STAT_FOCUS, STAT_SELF, 4, eTrainUnusable },
      { _T( "Leadership" ), STAT_SELF, STAT_NULL, 4, eTrainUnusable },
      { _T( "Loyalty" ), STAT_SELF, STAT_NULL, 4, eTrainUnusable },
      { _T( "Fletching" ), STAT_FOCUS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Alchemy" ), STAT_FOCUS, STAT_COORDINATION, 3, eTrainUnusable },
      { _T( "Cooking" ), STAT_FOCUS, STAT_COORDINATION, 3, eTrainUnusable }
};

const char *StatName[6] = { "Strength", "Endurance", "Coordination", "Quickness", "Focus", "Self" };
const char *SecondaryStatName[6] = { "Health", "Stamina", "Mana" };

void cACServer::Parse200(int UserParsing, DWORD MessageType, BYTE *Packet, int PacketLen)
{
	switch (MessageType)
	{
	case 0xF656:
		{
			//Character Creation!!
			DWORD beefbeef, tpdword;
			BYTE shit[0x170], *PackPointer;
			PackPointer = Packet;
			memcpy(&beefbeef, PackPointer, 4);	PackPointer += 4;
			memcpy(shit, PackPointer, 0x168);	PackPointer += 0x168;	//starts at $20
			
			WORD CharNameLen;
			BYTE CharName[32];
			memcpy(&CharNameLen, PackPointer, 2);			PackPointer += 2;
			memcpy(CharName, PackPointer, CharNameLen);		PackPointer += CharNameLen;

			DWORD CharNum;
			for (int i=0;i<5;i++)
			{
				if (!ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i])
				{
					memcpy(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[i], CharName,32);
					CalcNextFreeGUID();
					ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] = NextFreeCharGUID;
					CharNum = i;
					i = 5;

					SaveAccounts();
				}
			}

			//Fill out stCharType and send to WorldServer
			cWorldObject_Char * NewChar = new cWorldObject_Char();
			NewChar->GUID = ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[CharNum];
			memcpy(NewChar->Name, CharName, 32);
			sprintf(NewChar->Title, "Newb");
			
			//Read in shit
			PackPointer = &shit[0];
//					char ZN[20];		memcpy(ZN, PackPointer, 20);	PackPointer += 20;				//Unknown1
//					DWORD unk1;			memcpy(&unk1, PackPointer, 4);	PackPointer += 4;				//Unknown1
//										memcpy(&NewChar->Gender , PackPointer, 4);	PackPointer += 4;	//Guessing its Gender
//										memcpy(&NewChar->Race , PackPointer, 4);	PackPointer += 4;	//Race, Aluv/Gharu/Sho 0/1/2
			/*$70 unknown right now*/									PackPointer += 0xA4;
								memcpy(&NewChar->InitialStat[0], PackPointer, 6*4);	PackPointer += 6*4;
			/*$10 unknown right now*/									PackPointer += 0x10;
			

			sprintf(sayit, "* %s created a new char, %s with GUID: %08X", ConnUser[UserParsing].ZoneName, NewChar->Name , NewChar->GUID); LogDisp(sayit);

			ZeroMemory(NewChar->SkillInc, sizeof(NewChar->SkillInc));
			ZeroMemory(NewChar->SkillFreeXP, sizeof(NewChar->SkillFreeXP));
			ZeroMemory(NewChar->SkillXP, sizeof(NewChar->SkillXP));
			DWORD tpskill;
			for (i=0; i < 39; i++)
			{
				memcpy(&tpskill, PackPointer, 4);
				PackPointer += 4;
				NewChar->SkillTrain[i+1] = 0;
				if (tpskill)
				{
					NewChar->SkillTrain[i+1] = tpskill;
					if (tpskill == 1)
					{
						NewChar->SkillInc[i+1] = 0;
						NewChar->SkillFreeXP[i+1] = 0;
					}
					if (tpskill == 2)
					{
						NewChar->SkillInc[i+1] = 5;
						NewChar->SkillFreeXP[i+1] = 526;
					}
					if (tpskill == 3)
					{
						NewChar->SkillInc[i+1] = 10;
						NewChar->SkillFreeXP[i+1] = 671;
					}

					NewChar->SkillXP[i+1] = 0;
				}
			}
			
			NewChar->Vitae = 0;

			NewChar->Level = 1;

			//Eventually decode starting place... For now teth works. :)
			NewChar->Loc.landblock = 0x25810034;
			NewChar->Loc.x = 156.1f;
			NewChar->Loc.y = 81.9f;
			NewChar->Loc.z = 220.0f;

			tpdword = 0x3F579630; memcpy(&NewChar->Loc.a, &tpdword, 4);
			NewChar->Loc.b = 0x00000000;
			NewChar->Loc.c = 0x00000000;
			NewChar->Loc.w = 90.1f;	//Heading

			memcpy(&NewChar->LSTie, &NewChar->Loc, sizeof(NewChar->Loc));

			NewChar->CurrentSecondaryStat[0] = NewChar->InitialStat[1]/2;
			NewChar->CurrentSecondaryStat[1] = NewChar->InitialStat[1];
			NewChar->CurrentSecondaryStat[2] = NewChar->InitialStat[5];

			NewChar->BaseSecondaryStat[0] = NewChar->InitialStat[1]/2;
			NewChar->BaseSecondaryStat[1] = NewChar->InitialStat[1];
			NewChar->BaseSecondaryStat[2] = NewChar->InitialStat[5];

			ZeroMemory(NewChar->XPIntoSecondaryStat, sizeof(NewChar->XPIntoSecondaryStat));
			ZeroMemory(NewChar->XPIntoStat, sizeof(NewChar->XPIntoStat));
			ZeroMemory(NewChar->SecondaryStatInc, sizeof(NewChar->SecondaryStatInc));

			memcpy(NewChar->CurrentStat, NewChar->InitialStat, 6*4);

			ZeroMemory(NewChar->MainPack, sizeof(NewChar->MainPack));
			ZeroMemory(NewChar->SidePacks, sizeof(NewChar->SidePacks));
			ZeroMemory(NewChar->Equipped, sizeof(NewChar->Equipped));
			NewChar->Model = 1;
			NewChar->Monarch = 0;
			NewChar->NumLogins = 1;
			NewChar->Options = 0x01E8E543;
			NewChar->Patron = 0;
			NewChar->SkillCredits = 0;
			ZeroMemory(NewChar->SpellsLearned, sizeof(NewChar->SpellsLearned));
			NewChar->TotalXP = 0;
			NewChar->UnassignedXP = 0;
			NewChar->Vitae = 0;
			NewChar->Class = 0;
			ZeroMemory(NewChar->Vassal, sizeof(NewChar->Vassal));
			ZeroMemory(NewChar->ShortCut, sizeof(NewChar->ShortCut));
			ZeroMemory(NewChar->SpellsLearned, sizeof(NewChar->SpellsLearned));
			ZeroMemory(NewChar->SpellBar, sizeof(NewChar->SpellBar));
			NewChar->Scale = 1.0f;

			FILE *out = fopen("CharList.dat","a+b");

			fseek(out, 0, SEEK_END);
			
			BYTE TPlog[0x2000]; int charsize = NewChar->Serialize(TPlog);
			fwrite(&NewChar->GUID, 4, 1, out);
			fwrite(NewChar->Name, 32, 1, out);
			fwrite(&NewChar->Loc, sizeof(Location_t), 1, out);
			fwrite(TPlog, charsize, 1, out);

			fclose(out);

			sprintf(sayit, "New char created: %s with GUID %08X", NewChar->Name, NewChar->GUID); LogDisp(sayit);

			//Create Char Respond Packet...
			BYTE CreateChar[0x100];
			
			
			PackPointer = &CreateChar[0];

			tpdword = 0x0000F643;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
			tpdword = 0x00000001;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;	//Success
			tpdword = ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[CharNum];	memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
			int outlen; GenString(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[CharNum], &outlen, PackPointer); PackPointer += outlen;
			tpdword = 0x00000000;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;

			DWORD PackLen = (DWORD) ((DWORD) PackPointer - (DWORD) CreateChar);

			Send200(CreateChar, PackLen, 4, UserParsing);

			//Resend Char List!
			SendCharList(UserParsing);
		}
		break;
	case 0xF7C8:
		{
			//Entering Game
			UpdateCharInfo(UserParsing, "Got Enter Game Packet...");

			//Now to return a fun 200 packet... whee...
			DWORD tph3 = 0x0000F7C7;

			Send200((BYTE *) &tph3, sizeof(tph3), 4, UserParsing);

			ConnUser[UserParsing].State = 2;
		}
		break;
	case 0xF7D9:
		//Restoring Character
		{
			DWORD GUIDtoRestore; DWORD Ones[2];
			memcpy(&GUIDtoRestore, Packet,4);
			memcpy(&Ones[0], Packet+4,4);
			memcpy(&Ones[1], Packet+8,4);

			int CharRestoring = 0;
			for (int i=0;i<5;i++)
			{
				if ((ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] & 0x7FFFFFFF) == GUIDtoRestore)
				{
					ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[i] &= 0x7FFFFFFF;
					
					SaveAccounts();

					CharRestoring = i;
					i = 5;
				}
			}

			BYTE CreateChar[0x100], *PackPointer = &CreateChar[0];
			DWORD tpdword;

			tpdword = 0x0000F643;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
			tpdword = 0x00000001;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
			tpdword = GUIDtoRestore;	memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;

			tpdword = ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[CharRestoring] & 0x7FFFFFFF;
				memcpy(PackPointer,&tpdword,4); PackPointer += 4;	//GUID

			BYTE NameLen;
			NameLen = strlen(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[CharRestoring]) + 1;
				memcpy(PackPointer,&NameLen,sizeof(NameLen)); PackPointer += sizeof(NameLen);	//NameLen

			*PackPointer = 0; PackPointer++;												//0
			NameLen--; memcpy(PackPointer,&ZonePasses[ConnUser[UserParsing].ZoneNum].Names[CharRestoring],NameLen); PackPointer += NameLen;	//Name
			*PackPointer = 0; PackPointer++;												//0
			
			int Tpl = ((PackPointer-&CreateChar[0])%4); if (Tpl) Tpl=4-Tpl;			//Padding
			for (int ii=0;ii<Tpl;ii++) {	*PackPointer = 0; PackPointer++; }		//"
			
			tpdword = 0x00000000;		memcpy(PackPointer, &tpdword, 4);	PackPointer += 4;
			Send200(&CreateChar[0], (int) (PackPointer - &CreateChar[0]), 4, UserParsing);

			//Resend Char List!
			SendCharList(UserParsing);
		}
		break;
	case 0xF655:
		//Deleting Character
		{
			DWORD beefbeef; char ZoneName[20]; DWORD LastWord[2];
			memcpy(&beefbeef, Packet, 4);
			memcpy(&ZoneName, Packet+4, 20);
			memcpy(&LastWord[1], Packet+24,4);

			//LastWord[0] seems to be 01f7 all the time
			//LastWord[1] is the index of the char deleted, in order of chars sent, i think...
			
			ZonePasses[ConnUser[UserParsing].ZoneNum].Chars[LastWord[1]] |= 0x80000000;	//To be deleted
			ZonePasses[ConnUser[UserParsing].ZoneNum].Secs2Del[LastWord[1]] = 300;	//5 Minutes
			SaveAccounts();
			
			NeedToDelete = true;

			//Resend Char List!
			SendCharList(UserParsing);
		}
		break;
	case 0xF657:
		UpdateCharInfo(UserParsing, "Got Enter game request...");

		memcpy(&ConnUser[UserParsing].charGUID, Packet, 4);
		
		ConnUser[UserParsing].State = 3;
		break;
	case 0xF7B1:
		//Game event...
		ParseF7B1(UserParsing, *((DWORD *) (Packet+4)), Packet+8, PacketLen - 8);
		break;
	case 0xF653:
		//Exit world..
		{
			ConnUser[UserParsing].State = 1;
			ConnUser[UserParsing].EventCount = 0;

			//Disconnect Him
			sprintf(sayit, "Player %s Disconnected!", ConnUser[UserParsing].Char->Name);
			ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);

			if (ConnUser[UserParsing].Char)
			{
//				LBObjects[ConnUser[UserParsing].Char->Loc.landblock >> 16].erase(ConnUser[UserParsing].Char->GUID);

//				ObjectList.erase(ConnUser[UserParsing].Char->GUID);

				SaveCharacter(UserParsing);

				ConnUser[UserParsing].Char = 0;
			}
			
			//Return him to char select screen
			DWORD tph3 = 0x0000F653;

			Send200((BYTE *) &tph3, sizeof(tph3), 4, UserParsing);

			SendCharList(UserParsing);

			//Delete him from other people
			//Somehow...
		}
		break;
	case 0xF6EA:
		//Nobody knows...
		break;
	case 0xF7A9:
		//Client needs landblock
		
		ServerMessage(UserParsing, "* You don't have at least one of the landblocks in your new area.  !telemap to another location or try typing /render radius 5.", COLOR_RED);
		break;
	default:
		{
			char *tps2, *tps3 = new char[1 + PacketLen*5];
			ZeroMemory(tps3,1 + PacketLen*5);
			tps2 = tps3;
			for (unsigned int tpl=0; tpl < PacketLen; tpl++)
			{
				sprintf(tps2, "0x%02X ", Packet[tpl]);
				tps2+=5;
			}
			sprintf(sayit, "200/%04X Pack Data: %s", MessageType, tps3); LogDisp(sayit);
			delete [] tps3;
		}
		break;
	}
}

void cACServer::ParseF7B1(int UserParsing, DWORD MessageType, BYTE *Packet, int PacketLen)
{
	if ((!ConnUser[UserParsing].Char) && (MessageType != 0x1F))
		return;
	
	switch (MessageType)
	{
	case 0x001F:
		{
			ConnUser[UserParsing].State = 4;

			DWORD GUIDtoLoad = ConnUser[UserParsing].charGUID;

			ServerMessage(UserParsing, "Welcome to KillaServ v-9.432", COLOR_BLUE);
			ServerMessage(UserParsing, "* Type !help for a Features List!", COLOR_BLUE);

			//Load character...
			FILE *charread = fopen("CharList.dat","rb");

			if (!charread)
			{
				//doh
				sprintf(sayit, "Couldn't find Character File... Bad... Disconnecting User..."); LogDisp(sayit);
				
				return;			//Bad...
			}

			fseek(charread, 0, SEEK_END);
			int numchars = ftell(charread)/(SIZEOFCHAR+68);
			fseek(charread, 0, SEEK_SET);

			DWORD GUIDtemp; bool CharFound = false;
			for (int j=0;j<numchars;j++)
			{
				fseek(charread, j*(SIZEOFCHAR+68), SEEK_SET);
				fread(&GUIDtemp, 4, 1, charread);
				if (GUIDtemp == GUIDtoLoad)
				{
					fseek(charread, j*(SIZEOFCHAR+68), SEEK_SET);
					cWorldObject_Char *tpchar = new cWorldObject_Char();
					ObjectList[GUIDtemp] = tpchar;
					ConnUser[UserParsing].Char = tpchar;

					fread(&tpchar->GUID, 4, 1, charread);
					fread(tpchar->Name, 32, 1, charread);
					fread(&tpchar->Loc, sizeof(Location_t), 1, charread);
					BYTE tpl[5000];
					fread(tpl, SIZEOFCHAR, 1, charread);
					tpchar->LoadFrom(tpl);
					
					j = numchars;
					CharFound = true;
				}
			}

			fclose(charread);

			if (!CharFound)
			{
				sprintf(sayit, "Couldn't find Character's Entry... Bad... Disconnecting User..."); LogDisp(sayit);
				
				return;			//Bad...
			}

			LBObjects[ConnUser[UserParsing].Char->Loc.landblock >> 16][ConnUser[UserParsing].Char->GUID] = (cWorldObject *) ConnUser[UserParsing].Char;

			//Load Objects out of store...
			for (j=0;j<25;j++) {
				if (ConnUser[UserParsing].Char->Equipped[j]) {
					ConnUser[UserParsing].EquippedPtr[j] = ObjectList[ConnUser[UserParsing].Char->Equipped[j]];
					ConnUser[UserParsing].ObjectCache[ConnUser[UserParsing].Char->Equipped[j]] = ConnUser[UserParsing].EquippedPtr[j];
				} else j = 25;
			}
			for (j=0;j<7;j++) {
				if (ConnUser[UserParsing].Char->SidePacks[j]) {
					ConnUser[UserParsing].SidePacksPtr[j] = ObjectList[ConnUser[UserParsing].Char->SidePacks[j]];
					ConnUser[UserParsing].ObjectCache[ConnUser[UserParsing].Char->SidePacks[j]] = ConnUser[UserParsing].SidePacksPtr[j];
				} else j = 7;
			}
			for (j=0;j<102;j++) {
				if (ConnUser[UserParsing].Char->MainPack[j]) {
					ConnUser[UserParsing].MainPackPtr[j] = ObjectList[ConnUser[UserParsing].Char->MainPack[j]];
					ConnUser[UserParsing].ObjectCache[ConnUser[UserParsing].Char->MainPack[j]] = ConnUser[UserParsing].MainPackPtr[j];
				} else j = 102;
			}

			ConnUser[UserParsing].Char->Owner = UserParsing;

			//Hax0r...?
			ConnUser[UserParsing].Char->NumLogins = 3; //++;

			sprintf(sayit, "* Requested Login, Loaded char %08X...", ConnUser[UserParsing].Char->GUID);	LogDisp(sayit);
			
			if (UserParsing >= MaxUsers)
				MaxUsers = UserParsing+1;

			ConnUser[UserParsing].State = 5;

			UpdateCharInfo(UserParsing, "Sending Main Login Info...");

			//Try to login...
			int packlen;
			BYTE CharData[0x2000];

			//SetPackContents eventually here for any of the 7 packs..

			Sleep(500);

			//Send giant login packet
			packlen = Generate_F7B0_13(CharData, UserParsing);
			Send200(CharData, packlen, 4, UserParsing);

			Sleep(500);

			//Create him on other ppl..
			BYTE CreatePlayer[0x2000];
			packlen = ConnUser[UserParsing].Char->GenerateCreatePacket(CreatePlayer);

			for (int iii=0;iii<MaxUsers;iii++)
			{
				if ((ConnUser[iii].Connected) && ((ConnUser[iii].State == 6) || (iii == UserParsing)))
					Send200(&CreatePlayer[0], packlen, 3, iii);
			}

			unsigned char LOGIN_CHAR[8] = {
				0x46, 0xF7, 0x00, 0x00, 0x30, 0x9F, 0x03, 0x50,                                                   //(F÷__0__P)         - 0000
			};

			memcpy(&LOGIN_CHAR[4],&ConnUser[UserParsing].Char->GUID,4);
			Send200(&LOGIN_CHAR[0], sizeof(LOGIN_CHAR), 3, UserParsing);

			sprintf(sayit, "Player %s Joined!", ConnUser[UserParsing].Char->Name);
			ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);
			
			WORD BaseBlock = ConnUser[UserParsing].Char->Loc.landblock >> 16;
			SendLandblock(UserParsing, BaseBlock);
			SendLandblock(UserParsing, BaseBlock-1);
			SendLandblock(UserParsing, BaseBlock+1);
			SendLandblock(UserParsing, BaseBlock-256);
			SendLandblock(UserParsing, BaseBlock+256);
			SendLandblock(UserParsing, BaseBlock-1-256);
			SendLandblock(UserParsing, BaseBlock+1-256256);
			SendLandblock(UserParsing, BaseBlock-1+256);
			SendLandblock(UserParsing, BaseBlock+1+256);

			unsigned char MSG_00000002[16] = {
				0x4B, 0xF7, 0x00, 0x00, 0x30, 0x9F, 0x03, 0x50, 0x10, 0x04, 0x40, 0x00, 0x03, 0x00, 0x02, 0x00,   //(K÷__0__P__@_____) - 0000
			};
			
			//Exit Portal Mode
			memcpy(&MSG_00000002[4],&ConnUser[UserParsing].Char->GUID,4);
			Send200(&MSG_00000002[0], sizeof(MSG_00000002), 3, UserParsing);

			ConnUser[UserParsing].State = 6;
		}
		break;
	case 0xF753:
		{
			//High-Priority Movement Packet

			Location_t *loc = (Location_t *) Packet;

			MoveUser(UserParsing, loc, false, false);
		}
		break;
	case 0xF61C:
		{
			//Low-Priority Movement Packet
			if (!ConnUser[UserParsing].Char)
				return;

			DWORD *hex=(DWORD *)(Packet - 12);

			Location_t *loc;	DWORD skip = 0;
			DWORD hex3;
			for (hex3=hex[3];hex3>0;hex3>>=1)
			{	if (hex3 & 1) skip++;	}

			//Flags
			DWORD SpeedFlag = 0, TurnFlag = 0, SlideFlag = 0, MoveFlag = 0, EmoteFlag = 0;

			DWORD * hexat = &hex[4];
			cAnimStruct ASt;
			ASt.wAnimation_Family = 0x3D;
			ASt.wAnimation_to_Play = 0x0;
			ASt.fPlaySpeed = 1.0f;
			if (hex[3] & 0x00000001)
			{	//Normal Speed, Unset = Walking
				SpeedFlag = *hexat;
				hexat += 1; }
			if (hex[3] & 0x00000004)
			{	//Moving
				DWORD MoveFlag = *hexat;
				hexat += 1;

				if (MoveFlag == 0x45000005)
				{
					ASt.fPlaySpeed = 1.0f;
					if (SpeedFlag == 0)
						ASt.wAnimation_to_Play = 0x05;
					else if (SpeedFlag == 2)
						ASt.wAnimation_to_Play = 0x07;
				} else if (MoveFlag == 0x45000006) {
					ASt.fPlaySpeed = -1.0f;
					ASt.wAnimation_to_Play = 0x05;
				} else if ((MoveFlag & 0xFF000000) == 0x41000000) {
					ASt.wAnimation_to_Play = (WORD) (MoveFlag & 0xFF);
				} else if ((MoveFlag & 0xFF000000) == 0x43000000) {
					ASt.wAnimation_to_Play = (WORD) (MoveFlag & 0xFF);
				} else {
					sprintf(sayit, "MoveFlag: %08X", MoveFlag); LogDisp(sayit);
				}
			}
			if (hex[3] & 0x00000020)
			{	//Sliding
				DWORD SlideFlag = *hexat;
				hexat += 1;

				if (SlideFlag == 0x6500000F)
				{
					ASt.fPlaySpeed = 1.0f;
					ASt.wAnimation_to_Play = 0x0F;
				} else if (SlideFlag == 0x65000010) {
					ASt.fPlaySpeed = -1.0f;
					ASt.wAnimation_to_Play = 0x0F;
				}
			}
			if (hex[3] & 0x00000100)
			{	//Turning
				DWORD TurnFlag = *hexat;
				hexat += 1;
				
				if (TurnFlag == 0x6500000D)			//Right
				{
					ASt.fPlaySpeed = 1.0f;
					ASt.wAnimation_to_Play = 0x0D;
				} else if (TurnFlag == 0x6500000E) {	//Left
					ASt.fPlaySpeed = -1.0f;
					ASt.wAnimation_to_Play = 0x0D;
				}
			}
			if (hex[3] & 0x0000F800)
			{	//Emote Backlog
				DWORD EmoteFlag = *hexat;
				DWORD numEmotes = (hex[3] & 0x0000F800) >> 11;
				hexat += (numEmotes * 2);
				skip += (numEmotes * 2) - 1;
				if (numEmotes > 2)	//For some reason, on 3+ emotes, it has another DWORD at the end
					skip++;
				
				//Just play the first one
				ASt.wAnimation_to_Play = (WORD) (EmoteFlag & 0xFF);
				if (numEmotes > 1)
				{
					//Lots of emotes...  Learn how to chain later! =]
				}
			}

			loc=(Location_t *)&hex[4+skip];
			MoveUser(UserParsing, loc, false, false);

			//Make Anim Packet... Ho ho ho!
			ASt.dwf74c = 0x0000F74C;
			ASt.dwGUID = ConnUser[UserParsing].Char->GUID;
			ASt.wNumInteractions = ConnUser[UserParsing].move_count;
			ASt.wNumLogins = ConnUser[UserParsing].Char->NumLogins;

			ConnUser[UserParsing].AnimCount++;
			ASt.wNumAnimations = ConnUser[UserParsing].AnimCount;
			ASt.wAnimation_Seqnum = ConnUser[UserParsing].AnimCount;

			ASt.wFlag = 0;
			ASt.wUnk1 = 0;
			ASt.GUIDTarget = 0x00000080;

			for (int iii=0;iii<MaxUsers;iii++)
			{
				if ((ConnUser[iii].Connected) && (ConnUser[iii].State == 6) && (iii != UserParsing))
					Send200((BYTE *) &ASt, sizeof(ASt), 3, iii);
			}

			cAnimStruct AStA;
			memcpy(&AStA, &ASt, sizeof(AStA));
			AStA.wFlag = 1;
			AStA.fPlaySpeed = ASt.fPlaySpeed;
			Send200((BYTE *) &AStA, sizeof(AStA), 3, UserParsing);

			ConnUser[UserParsing].move_count++;
		}
		break;
	case 0x0008:
		{
			//Player Attacked
			DWORD PlayerAttacked; memcpy(&PlayerAttacked, Packet, 4);
			DWORD AttackSpot; memcpy(&AttackSpot, Packet + 4, 4);
			DWORD Slider; memcpy(&Slider, Packet + 8, 4);
			
			//Spot: Low/mid/high, 3/2/1

			cWorldObject_Char *tpc = (cWorldObject_Char *) ObjectList[PlayerAttacked];
			if (tpc)
			{
				//... bad...?
				return;
			}
			DWORD TotalSkill = ConnUser[UserParsing].Char->SkillInc[0x0D] + (ConnUser[UserParsing].Char->CurrentStat[0] + ConnUser[UserParsing].Char->CurrentStat[2])/3;
			DWORD DamageDone = (rand() & 3) + (TotalSkill / 10) + (ConnUser[UserParsing].Char->CurrentStat[0]/20);
			
			tpc->CurrentSecondaryStat[0] -= DamageDone;

			//Damage Packet

			//Recieve
			BYTE DamagePacket[100], *PackPointer = DamagePacket;

			DWORD tpd = 0xF7B0;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
								memcpy(PackPointer, &tpc->GUID, 4);	PackPointer += 4;
								memcpy(PackPointer, &ConnUser[tpc->Owner].EventCount, 4);	PackPointer += 4;
			tpd = 0x01b2;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//13 = Login Char
			ConnUser[tpc->Owner].EventCount++;
			int strlen; GenString(ConnUser[UserParsing].Char->Name, &strlen, PackPointer);	PackPointer += strlen;

			DWORD damage = 4; memcpy(PackPointer, &damage, 4); PackPointer += 4;
			double severity = 0.5f; memcpy(PackPointer, &severity, 8); PackPointer += 8;
						memcpy(PackPointer, &DamageDone, 4); PackPointer += 4;
			Send200(DamagePacket, (DWORD) PackPointer - (DWORD) DamagePacket, 4, tpc->Owner);
			
			//Inflict
			PackPointer = DamagePacket;

			tpd = 0xF7B0;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
								memcpy(PackPointer, &ConnUser[UserParsing].Char->GUID, 4);	PackPointer += 4;
								memcpy(PackPointer, &ConnUser[UserParsing].EventCount, 4);	PackPointer += 4;
			tpd = 0x01b1;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//13 = Login Char
			ConnUser[UserParsing].EventCount++;
			GenString(tpc->Name, &strlen, PackPointer);	PackPointer += strlen;

			memcpy(PackPointer, &damage, 4); PackPointer += 4;
			memcpy(PackPointer, &severity, 8); PackPointer += 8;
						memcpy(PackPointer, &DamageDone, 4); PackPointer += 4;
			DWORD spot;
			switch (AttackSpot)
			{
			case 1:
				//High
				spot = rand() & 1;
				break;
			case 2:
				//Mid
				spot = 1 + (rand() % 5);
				break;
			case 3:
				//Low
				spot = 7 + (rand() & 1);
				break;
			}
							memcpy(PackPointer, &spot, 4); PackPointer += 4;

			Send200(DamagePacket, (DWORD) PackPointer - (DWORD) DamagePacket - 4, 4, UserParsing);

			//Attack Completed
			DWORD tp1a7[] = {
				0x0000F7B0,
				ConnUser[UserParsing].Char->GUID,
				ConnUser[UserParsing].EventCount,
				0x01A7,
				ConnUser[UserParsing].AttackCount
			};
			Send200((BYTE *) tp1a7, sizeof(tp1a7), 4, UserParsing);
			ConnUser[UserParsing].AttackCount++;
			ConnUser[UserParsing].EventCount++;

			if ((int) tpc->CurrentSecondaryStat[0] <= 0)
			{
				//Dead ktnxunf
				sprintf(sayit, "%s was 0wn3d by %s!", tpc->Name, ConnUser[UserParsing].Char->Name);
				ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);

				tpc->CurrentSecondaryStat[0] = (tpc->CurrentStat[1]/2) + tpc->SecondaryStatInc[0];

				MoveUser(tpc->Owner, &tpc->LSTie, true, false);
			}
		}
		break;
	case 0x0044:
		{
			//Player raised secondary stat
			//DWORD 1 = stat number?
			//DWORD 2 = ?

			DWORD dword1; memcpy(&dword1, Packet, 4);
			DWORD dword2; memcpy(&dword2, Packet + 4, 4);
			dword1--;
			dword1 >>= 1;

			DWORD oldv = ConnUser[UserParsing].Char->SecondaryStatInc[dword1];

			ConnUser[UserParsing].Char->UnassignedXP -= SecondaryStatArray[oldv+1] - ConnUser[UserParsing].Char->XPIntoSecondaryStat[dword1];

			ConnUser[UserParsing].Char->SecondaryStatInc[dword1]++;
			ConnUser[UserParsing].Char->XPIntoSecondaryStat[dword1] = SecondaryStatArray[oldv+1];

			DWORD tpdword;	BYTE tpbyte;
			//Unassigned XP update
			BYTE Pack237[200];
			ZeroMemory(Pack237, 0xD);
			tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
			tpbyte = ConnUser[UserParsing].Count237[0x16];	memcpy(&Pack237[4], &tpbyte, 1);
			tpdword = 0x16;	 memcpy(&Pack237[5], &tpdword, 4);		//Unassigned XP
			tpdword = ConnUser[UserParsing].Char->UnassignedXP;  memcpy(&Pack237[9], &tpdword, 4);
			Send200(Pack237, 13, 4, UserParsing);
			ConnUser[UserParsing].Count237[0x16]++;

			UpdateSecondaryStat(UserParsing, dword1);

			//Notice
			char SrvMsg[100]; sprintf(SrvMsg, "Your base %s is now %i!", SecondaryStatName[dword1], ConnUser[UserParsing].Char->BaseSecondaryStat[dword1]);
			ServerMessage(UserParsing, SrvMsg, COLOR_BLUE);
			SendSound(UserParsing, 0x76, 1.0f);
		}
		break;
	case 0x0045:
		{
			//Player raised stat
			//DWORD 1 = stat number?
			//DWORD 2 = ?

			DWORD dword1; memcpy(&dword1, Packet, 4);
			DWORD dword2; memcpy(&dword2, Packet + 4, 4);

			dword1--;
			if (dword1 == 3) dword1 = 2;
			else if (dword1 == 2) dword1 = 3;

			DWORD oldv = ConnUser[UserParsing].Char->CurrentStat[dword1] - ConnUser[UserParsing].Char->InitialStat[dword1];

			ConnUser[UserParsing].Char->UnassignedXP -= PrimStatArray[oldv+1] - ConnUser[UserParsing].Char->XPIntoStat[dword1];

			ConnUser[UserParsing].Char->CurrentStat[dword1]++;
			ConnUser[UserParsing].Char->XPIntoStat[dword1] = PrimStatArray[oldv+1];

			DWORD tpdword;	BYTE tpbyte;
			//Unassigned XP update
			BYTE Pack237[200];
			ZeroMemory(Pack237, 0xD);
			tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
			tpbyte = ConnUser[UserParsing].Count237[0x16];	memcpy(&Pack237[4], &tpbyte, 1);
			tpdword = 0x16;	 memcpy(&Pack237[5], &tpdword, 4);		//Unassigned XP
			tpdword = ConnUser[UserParsing].Char->UnassignedXP;  memcpy(&Pack237[9], &tpdword, 4);
			Send200(Pack237, 13, 4, UserParsing);
			ConnUser[UserParsing].Count237[0x16]++;

			UpdateStat(UserParsing, dword1);

			//Notice
			char SrvMsg[100]; sprintf(SrvMsg, "Your base %s is now %i!", StatName[dword1], ConnUser[UserParsing].Char->CurrentStat[dword1]);
			ServerMessage(UserParsing, SrvMsg, COLOR_BLUE);

			SendSound(UserParsing, 0x76, 1.0f);
		}
		break;
	case 0x0046:
		{
			//Player raised skill
			//DWORD 1 = skill number
			//DWORD 2 = ?
			DWORD dword1; memcpy(&dword1, Packet, 4);
			DWORD dword2; memcpy(&dword2, Packet + 4, 4);

			DWORD oldv = ConnUser[UserParsing].Char->SkillInc[dword1];

			if (ConnUser[UserParsing].Char->SkillTrain[dword1] == 3)
			{
					ConnUser[UserParsing].Char->UnassignedXP -= SpecSkillArray[oldv+1] - ConnUser[UserParsing].Char->SkillXP[dword1] - ConnUser[UserParsing].Char->SkillFreeXP[dword1];

				ConnUser[UserParsing].Char->SkillInc[dword1]++;
				ConnUser[UserParsing].Char->SkillXP[dword1] = SpecSkillArray[oldv+1] - ConnUser[UserParsing].Char->SkillFreeXP[dword1];
			} else {
				ConnUser[UserParsing].Char->UnassignedXP -= TrainedSkillArray[oldv+1] - ConnUser[UserParsing].Char->SkillXP[dword1] - ConnUser[UserParsing].Char->SkillFreeXP[dword1];

				ConnUser[UserParsing].Char->SkillInc[dword1]++;
				ConnUser[UserParsing].Char->SkillXP[dword1] = TrainedSkillArray[oldv+1] - ConnUser[UserParsing].Char->SkillFreeXP[dword1];
			}

			DWORD tpdword;	BYTE tpbyte;
			//Unassigned XP update
			BYTE Pack237[200];
			ZeroMemory(Pack237, 0xD);
			tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
			tpbyte = ConnUser[UserParsing].Count237[0x16];	memcpy(&Pack237[4], &tpbyte, 1);
			tpdword = 0x16;	 memcpy(&Pack237[5], &tpdword, 4);		//Unassigned XP
			tpdword = ConnUser[UserParsing].Char->UnassignedXP;  memcpy(&Pack237[9], &tpdword, 4);
			Send200(Pack237, 13, 4, UserParsing);
			ConnUser[UserParsing].Count237[0x16]++;

			UpdateSkill(UserParsing, dword1);

			//Notice
			char SrvMsg[100]; sprintf(SrvMsg, "Your base %s skill is now %i!", SkillInfo[dword1].Name, ConnUser[UserParsing].Char->SkillBase[dword1]);
			ServerMessage(UserParsing, SrvMsg, COLOR_BLUE);

			SendSound(UserParsing, 0x76, 1.0f);
		}
		break;
	case 0x0047:
		{
			//Player trained skill
			//DWORD 1 = skill number
			//DWORD 2 = ?
			DWORD dword1; memcpy(&dword1, Packet, 4);
			DWORD dword2; memcpy(&dword2, Packet + 4, 4);
			
			//subtract skill points
			ConnUser[UserParsing].Char->SkillCredits -= CreditCost[dword1];
			//raise skill to trained
			ConnUser[UserParsing].Char->SkillTrain[dword1] = 2;

			//Skill credit number
			BYTE Pack237[0xD];
			DWORD tpdword;	BYTE tpbyte;
			ZeroMemory(Pack237, 0xD);
			tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
			tpbyte = ConnUser[UserParsing].Count237[0x18];	memcpy(&Pack237[4], &tpbyte, 1);
			tpdword = 0x18;	 memcpy(&Pack237[5], &tpdword, 4);		//Credits
			tpdword = ConnUser[UserParsing].Char->SkillCredits;  memcpy(&Pack237[9], &tpdword, 4);
			Send200(Pack237, 13, 4, UserParsing);
			ConnUser[UserParsing].Count237[0x18]++;

			UpdateSkill(UserParsing, dword1);

			SendSound(UserParsing, 0x76, 1.0f);
		}
		break;
	case 0x0036:
		{
			//Player used item
			DWORD tpguid; memcpy(&tpguid, Packet, 4);
			cWorldObject * tpitem = ObjectList[tpguid];
			switch (tpitem->Type)
			{
			case OBJECT_LIFESTONE:
				memcpy(&ConnUser[UserParsing].Char->LSTie, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));
				ServerMessage(UserParsing, "You successfully tie to the lifestone!", COLOR_BLUE);
				SendAnim(UserParsing, 0x0036, 1.0f);
				break;
			case OBJECT_PORTAL:
				PointAt(UserParsing, tpguid);
//									SendAnim(UserParsing, 7);	//Run
//									ActionComplete(UserParsing);
				break;
			}
		}
		break;
	case 0x0063:
		//Player /lifestoned :)
		sprintf(sayit, "%s died!", ConnUser[UserParsing].Char->Name);
		ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);

//		SendAnim(UserParsing, 0x0008);
		
		MoveUser(UserParsing, &ConnUser[UserParsing].Char->LSTie, true, false);
		break;
	case 0x00A1:
		{
			//Character logged in...
			
			UpdateCharInfo(UserParsing, "Fully Logged In");

			//Fully log him in on others
			DWORD F74B[] = {
				0x0000F74B,
				ConnUser[UserParsing].Char->GUID,
				0x00400410,
				0x00020003
			};
//					memcpy(&F74B[3], &ConnUser[UserParsing].Char->NumLogins, 2);
//					memcpy((&F74B[3]) + 2, &ConnUser[UserParsing].PortalCount, 2);

			for (int iii=0;iii<MaxUsers;iii++)
			{
				if ((ConnUser[iii].Connected) && (ConnUser[iii].State == 6))
					Send200((BYTE *) F74B, sizeof(F74B), 3, iii);
			}
			
			BYTE CreatePlayer[0x500];	int packlen;
			for (iii=0;iii<MaxUsers;iii++)
			{
				if ((ConnUser[iii].Connected) && (ConnUser[iii].State == 6) && (iii != UserParsing))
				{
					packlen = ConnUser[iii].Char->GenerateCreatePacket(CreatePlayer);
					//Show others on him
					Send200(CreatePlayer, packlen, 3, UserParsing);

					memcpy(&F74B[1], &ConnUser[iii].Char->GUID, 4);
					Send200((BYTE *) &F74B[0], sizeof(F74B), 3, UserParsing);
				}
			}
		}
		break;
	case 0x0015:
		{
			//Text Message from Client
			if (PacketLen > 0)
			{
				char messagetext[2000];
				memcpy(messagetext,Packet+2,PacketLen-2);

				if (*messagetext == '!')
				{
					//Command
					char command[50], *Contents; ZeroMemory(command, 50);
//										char seps[] = " ";
					if (strstr(messagetext, " "))
					{
						memcpy(command,messagetext,(DWORD) strstr(messagetext, " ") - (DWORD) messagetext);
						Contents = strstr(messagetext," ");
					} else {
						memcpy(command,messagetext,strlen(messagetext));
						Contents = 0;
					}
					if (Contents)
						Contents = strstr(Contents, " ") + 1;
					
					int NumParms = 0;
					char Parms[10][50];	ZeroMemory(Parms, 10*50);
					if ((DWORD) Contents > 1)
					{
						char *tp = Contents, *tp2;
						while ((DWORD) tp > 1)
						{
							tp2 = strstr(tp," ") + 1;
							if ((DWORD) tp2 <= 1)
							{
								memcpy(Parms[NumParms], tp, strlen(tp));
								NumParms++;
								tp = tp2;
							} else {
								memcpy(Parms[NumParms], tp, (DWORD) (tp2 - tp - 1));
								NumParms++;
								tp = tp2;
							}
						}
					}

					if (stricmp(&command[1],"tele") == 0)	//Teleport by landblock
					{
						try {
							Location_t templ;
							memcpy(&templ, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));

							if (NumParms < 4)
							{
								ServerMessage(UserParsing, "Error in !tele syntax, check !help!", COLOR_RED);
								return;
							}
							//Fill in new Loc...
							sscanf(Parms[0],"%f",&templ.x);
							sscanf(Parms[1],"%f",&templ.y);
							sscanf(Parms[2],"%f",&templ.z);
							sscanf(Parms[3],"%08X",&templ.landblock);

							MoveUser(UserParsing, &templ, true, false);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !tele syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"destroy") == 0) {
						RemoveItem(ConnUser[UserParsing].SelectedItem);
						cWorldObject *hm = ObjectList[ConnUser[UserParsing].SelectedItem];
					} else if (stricmp(&command[1],"destroyo") == 0) {
						DWORD tpgr;
						sscanf(Parms[0], "%08X", &tpgr);
						cWorldObject *hm = ObjectList[tpgr];
						if (hm)
							RemoveItem(hm->GUID);
					} else if (stricmp(&command[1],"testswap") == 0) {
						BYTE F625[500], *PP = F625;

						//F625
						*((DWORD *)PP)	= 0xF625;	PP += 4;
						
						//Header
						*((BYTE *)PP)	= 0x11;		PP += 1;
						*((BYTE *)PP)	= 3;		PP += 1;
						*((BYTE *)PP)	= 4;		PP += 1;
						*((BYTE *)PP)	= 17;		PP += 1;

						//PaletteSwaps
						*((WORD *)PP)	= 0x007E;	PP += 2;
						*((BYTE *)PP)	= 0xAD;		PP += 1;
						*((BYTE *)PP)	= 0x04;		PP += 1;
						*((WORD *)PP)	= 0x1800;	PP += 2;
						*((BYTE *)PP)	= 0xF9;		PP += 1;
						*((BYTE *)PP)	= 0x02;		PP += 1;
						*((WORD *)PP)	= 0x0818;	PP += 2;
						*((BYTE *)PP)	= 0xAF;		PP += 1;
						*((BYTE *)PP)	= 0x04;		PP += 1;
						//PaletteUnknown
						*((WORD *)PP)	= 0x0820;	PP += 2;

						//TextureSwaps
						*((BYTE *)PP)	= 10;		PP += 1;
						*((WORD *)PP)	= 0x0098;	PP += 2;
						*((WORD *)PP)	= 0x11FD;	PP += 2;
						*((BYTE *)PP)	= 10;		PP += 1;
						*((WORD *)PP)	= 0x024C;	PP += 2;
						*((WORD *)PP)	= 0x1154;	PP += 2;
						*((BYTE *)PP)	= 10;		PP += 1;
						*((WORD *)PP)	= 0x02F5;	PP += 2;
						*((WORD *)PP)	= 0x1157;	PP += 2;
						*((BYTE *)PP)	= 10;		PP += 1;
						*((WORD *)PP)	= 0x025C;	PP += 2;
						*((WORD *)PP)	= 0x11DB;	PP += 2;
						
						//ModelSwaps
						*((BYTE *)PP)	= 0;		PP += 1;
						*((WORD *)PP)	= 0x004E;	PP += 2;
						*((BYTE *)PP)	= 1;		PP += 1;
						*((WORD *)PP)	= 0x004F;	PP += 2;
						*((BYTE *)PP)	= 2;		PP += 1;
						*((WORD *)PP)	= 0x004D;	PP += 2;
						*((BYTE *)PP)	= 5;		PP += 1;
						*((WORD *)PP)	= 0x0053;	PP += 2;
						*((BYTE *)PP)	= 6;		PP += 1;
						*((WORD *)PP)	= 0x0051;	PP += 2;
						*((BYTE *)PP)	= 9;		PP += 1;
						*((WORD *)PP)	= 0x0054;	PP += 2;
						*((BYTE *)PP)	= 0xA;		PP += 1;
						*((WORD *)PP)	= 0x0497;	PP += 2;
						*((BYTE *)PP)	= 0xB;		PP += 1;
						*((WORD *)PP)	= 0x0495;	PP += 2;
						*((BYTE *)PP)	= 0xC;		PP += 1;
						*((WORD *)PP)	= 0x0076;	PP += 2;
						*((BYTE *)PP)	= 0xD;		PP += 1;
						*((WORD *)PP)	= 0x04AD;	PP += 2;
						*((BYTE *)PP)	= 0xE;		PP += 1;
						*((WORD *)PP)	= 0x0496;	PP += 2;
						*((BYTE *)PP)	= 0xF;		PP += 1;
						*((WORD *)PP)	= 0x0077;	PP += 2;
						*((BYTE *)PP)	= 3;		PP += 1;
						*((WORD *)PP)	= 0x004C;	PP += 2;
						*((BYTE *)PP)	= 7;		PP += 1;
						*((WORD *)PP)	= 0x0050;	PP += 2;
						*((BYTE *)PP)	= 4;		PP += 1;
						*((WORD *)PP)	= 0x004B;	PP += 2;
						*((BYTE *)PP)	= 8;		PP += 1;
						*((WORD *)PP)	= 0x0052;	PP += 2;
						*((BYTE *)PP)	= 0x10;		PP += 1;
						*((WORD *)PP)	= 0x049E;	PP += 2;

						//ModelSequenceType
						*((WORD *)PP)	= 0x0564;	PP += 2;

						//ModelSequence
						*((WORD *)PP)	= ConnUser[UserParsing].ModelCount;	PP += 2;

						for (int i=1;i<10;i++)
							Send200(F625, (int) (PP - F625), i, UserParsing);
					} else if (stricmp(&command[1],"tswap") == 0) {
						int A, B, C;
						sscanf(Parms[0], "%02X", &A);
						sscanf(Parms[1], "%04X", &B);
						sscanf(Parms[2], "%04X", &C);
						ConnUser[UserParsing].Char->OldTexSwap[A] = B;
						ConnUser[UserParsing].Char->TexSwap[A] = C;
						BYTE hehe[0x500]; int pl = ConnUser[UserParsing].Char->GenerateTexChangePacket(hehe);
						Send200(hehe, pl, 3, UserParsing);
						Send200(hehe, pl, 4, UserParsing);
					} else if (stricmp(&command[1],"telemap") == 0) {
						try {
							if (NumParms < 2)
							{
								ServerMessage(UserParsing, "Error in !telemap syntax, check !help!", COLOR_RED);
								return;
							}
							float NS, EW;//, Nx, Ny;
							char tpt;
							tpt = Parms[0][strlen(Parms[0])-1];
							Parms[0][strlen(Parms[0])-1] = 0;
							if ((tpt == 'N') || (tpt == 'n'))
							{
								sscanf(Parms[0],"%f",&NS);
							} else if ((tpt == 'S') || (tpt == 's')) {
								sscanf(Parms[0],"%f",&NS);
								NS *= -1;
							} else {
								NS = 0;
							}

							tpt = Parms[1][strlen(Parms[1])-1];
							Parms[1][strlen(Parms[1])-1] = 0;
							if ((tpt == 'E') || (tpt == 'e'))
							{
								sscanf(Parms[1],"%f",&EW);
							} else if ((tpt == 'W') || (tpt == 'w')) {
								sscanf(Parms[1],"%f",&EW);
								EW *= -1;
							} else {
								EW = 0;
							}

							Location_t templ;
							
							templ.x = (EW*10.0f+1020.0f)*24.0f;
							templ.y = (NS*10.0f-1020.0f)*24.0f;
							if (NumParms == 3)
								sscanf(Parms[2], "%f", &templ.z);
							else
								templ.z = 0.0f;
							
							templ.landblock = 
									( (BYTE) ((((DWORD) templ.x%192)/24)*8) + (((DWORD) templ.y%192)/24) ) |
									( (0x00) << 8) |
									( (BYTE) (templ.y/192.0f) << 16) |
									( (BYTE) (templ.x/192.0f) << 24)
								;
							templ.landblock -= 0x00010000;

							templ.x = (float) ((int) templ.x % 192);
							templ.y = (float) ((int) templ.y % 192);

							MoveUser(UserParsing, &templ, true, false);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !telemap syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"srvmsg") == 0) {
						try {
							sprintf(sayit, "Server Message from %s: %s", ConnUser[UserParsing].Char->Name, Contents);
							ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !srvmsg syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"who") == 0) {
						//Send list of people
						ServerMessage(UserParsing, "Listing Players:", COLOR_GREEN);
						for (int i=0;i<MaxUsers;i++)
						{
							if (ConnUser[i].Connected)
							{
								if (ConnUser[i].Char)
								{
									sprintf(sayit, "* %i - %i.%i.%i.%i:%i - %s - %s",
										i,
										ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b1,
										ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b2,
										ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b3,
										ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b4,
										ntohs(ConnUser[i].sockaddy.sin_port),
										ConnUser[i].Char->Name,
										ConnUser[i].Status);
								} else {
								sprintf(sayit, "* %i - %i.%i.%i.%i:%i - %s - %s",
									i,
									ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b1,
									ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b2,
									ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b3,
									ConnUser[i].sockaddy.sin_addr.S_un.S_un_b.s_b4,
									ntohs(ConnUser[i].sockaddy.sin_port),
									"*At Char Select*",
									ConnUser[i].Status);
								}
								ServerMessage(UserParsing, sayit, COLOR_GREEN);
							}
						}
					} else if (stricmp(&command[1],"kick") == 0) {
						try {	
							DWORD tokick;
							sscanf(Contents, "%i", &tokick);
							if ((tokick < 128) && (ConnUser[tokick].Connected))
							{
								DisconnectUser(tokick);
							} else {
								//Try by name...
								for (int ii=0;ii<MaxUsers;ii++)
								{
									if (stricmp(Contents, ConnUser[ii].Char->Name) == 0)
									{	DisconnectUser(ii); break; }
								}
							}
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !kick syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"help") == 0) {
						if (NumParms == 0)
						{
							ServerMessage(UserParsing, "ACServer Help:", COLOR_BLUE);
							ServerMessage(UserParsing, "Use !help [section] to get help on a section of commands...", COLOR_YELLOW);
							ServerMessage(UserParsing, " * Teleport - Basic teleporting commands\n\
 * Admin - Administrator Commands\n\
 * Effects - Special Effect Commands\n\
 * Avatar - Commands to mess with your avatar", COLOR_YELLOW);
						} else {
							if (stricmp(Parms[0],"teleport") == 0)
							{
								ServerMessage(UserParsing, "-=* Teleport *=-", COLOR_YELLOW);
						ServerMessage(UserParsing, " * !tele [X] [Y] [Z] [Landblock] - Teleport by Location_T\n\
 * !telemap [xx.xN/S] [xx.xE/W] [(Z)] - Teleport by NSEW coords with optional Z\n\
 * !telemapother [player number] [xx.xN/S] [xx.xE/W] [(Z)] - Teleports [player number] by NSEW coords with optional Z\n\
 * !goto [player number] - Teleports you to [player number]\n\
 * !summon [player number] - Summons [player number] to you", COLOR_GREEN);
							}
							else if (stricmp(Parms[0],"admin") == 0)
							{
								ServerMessage(UserParsing, "-=* Admin *=-", COLOR_YELLOW);
						ServerMessage(UserParsing, " * !givexp [player number] [Amount] - Give player unassigned XP\n\
 * !setskillinc [player number] [skill number] [new inc] - Changes player's skill increase number\n\
 * !srvmsg [message] - Server Message\n\
 * !kick [player number] - Kick player by number\n\
 * !create [named item] - Create Objects. Use '!create help' for specific syntax\n\
 * !smite [player number] - Hmmm...\n \
 * !who - List of Users Online (Gets Player Numbers)", COLOR_GREEN);
							}
							else if (stricmp(Parms[0],"effects") == 0)
							{
								ServerMessage(UserParsing, "-=* Effects *=-", COLOR_YELLOW);
								ServerMessage(UserParsing, " * !anim [anim number (0 to FFFF, hex)] - Make your character do animations\n\
 * !sound [sound number (0 to FFFF, hex)] - Make sound effects come from your character\n\
 * !particle [particle number (0 to FFFF, hex)] - Make particle effects come from your character", COLOR_GREEN);
							}
							else if (stricmp(Parms[0],"avatar") == 0)
							{
								ServerMessage(UserParsing, "-=* Avatar *=-", COLOR_YELLOW);
								ServerMessage(UserParsing, " * !changename [new name] - Change Player Name\n\
 * !changetitle [new title] - Changes your character title\n\
 * !changescale [new scale] - Changes the scale of your avatar model\n\
 * !changemodel [new model #] - Change your character to this model #", COLOR_GREEN);
							}
							else
							{
								ServerMessage(UserParsing, "Invalid Help Section, see !help...", COLOR_RED);
							}							
						}
					} else if (stricmp(&command[1],"changemodel") == 0) {
						WORD NewModel;
						sscanf(Parms[0], "%i", &NewModel);
						ConnUser[UserParsing].Char->Model = NewModel;
						ServerMessage(UserParsing, "Model changed!  Relog to see the effect...", COLOR_RED);
					} else if (stricmp(&command[1],"changescale") == 0) {
						float NewScale;
						sscanf(Parms[0], "%f", &NewScale);
						ConnUser[UserParsing].Char->Scale = NewScale;
						ServerMessage(UserParsing, "Scale changed!  Relog to see the effect...", COLOR_RED);
					} else if (stricmp(&command[1],"changetitle") == 0) {
						memcpy(ConnUser[UserParsing].Char->Title, Contents, 32);
						ServerMessage(UserParsing, "Title changed!  Relog to see the effect...", COLOR_RED);
					} else if (stricmp(&command[1],"changename") == 0) {
						char NewName[32];
						memcpy(NewName, Contents, 32);
						for (int i=0;i<5;i++)
						{
							if (strcmp(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[i],ConnUser[UserParsing].Char->Name) == 0)
							{
								strcpy(ZonePasses[ConnUser[UserParsing].ZoneNum].Names[i], NewName);
								i = 5;
							}
						}
						
						strcpy(ConnUser[UserParsing].Char->Name, NewName);

						ServerMessage(UserParsing, "Name changed!  Relog to see the effect...", COLOR_RED);
					} else if (stricmp(&command[1],"givexp") == 0) {
						DWORD PlayerFor;
						sscanf(Parms[0], "%i", &PlayerFor);
						DWORD AmounttoGive;
						sscanf(Parms[1], "%u", &AmounttoGive);

						if (PlayerFor > 128)
							return;

						GiveXP(PlayerFor, 128, AmounttoGive);
					} else if (stricmp(&command[1],"setskillinc") == 0) {
						DWORD PlayerFor;
						sscanf(Parms[0], "%i", &PlayerFor);
						DWORD SkillNum;
						sscanf(Parms[1], "%i", &SkillNum);
						DWORD AmounttoGive;
						sscanf(Parms[2], "%u", &AmounttoGive);

						if (PlayerFor > 128)
							return;

						if (SkillNum >= 40)
							return;

						if ((ConnUser[PlayerFor].Char) && (ConnUser[PlayerFor].State >= 5))
						{
							ConnUser[PlayerFor].Char->SkillInc[SkillNum] = AmounttoGive;
							UpdateSkill(PlayerFor, SkillNum);
						}
					} else if (stricmp(&command[1],"damage") == 0) {
						BYTE DamagePacket[100], *PackPointer = DamagePacket;

						DWORD head = 0x1b2; memcpy(PackPointer, &head, 4); PackPointer += 4;
						int strlen; GenString("Someone", &strlen, PackPointer);	PackPointer += strlen;

						DWORD damage = 4; memcpy(PackPointer, &damage, 4); PackPointer += 4;
						double severity = 0.5f; memcpy(PackPointer, &severity, 8); PackPointer += 8;
						DWORD amount = rand() & 7; memcpy(PackPointer, &amount, 4); PackPointer += 4;

						Send200(DamagePacket, (DWORD) PackPointer - (DWORD) DamagePacket, 4, UserParsing);
					
					} else if (stricmp(&command[1],"create") == 0) {
						DWORD tpguid; bool freeg = false;
						while (!freeg)
						{
							tpguid = 0x78000000 | ((rand()*rand()) & 0x00FFFFFF);
							if (ObjectList[tpguid] == 0)
								freeg = true;
						}

						if (!Contents)
						{
							ServerMessage(UserParsing, "Try !create help...", COLOR_BLUE);
							return;
						}

						if (stricmp(Contents, "help") == 0)
						{
							ServerMessage(UserParsing, "!create currently supports the following objects: lifestone, obj, portal, portalloc.", COLOR_BLUE);
							ServerMessage(UserParsing, " * Lifestone - Creates a lifestone in front of you\n\
* Obj - Creates generic objects... See !create obj help\n\
* Portal [name] [loc] - Creates portal by //loc address, [loc] is same format as !tele\n\
* Portalloc [name] [loc] - Creates portal by NSEW coords, [loc] is same format as !telemap", COLOR_GREEN);
						} else if (stricmp(Parms[0], "portalloc") == 0) {
							cWorldObject_Portal *tpprt = new cWorldObject_Portal();
							
							tpprt->GUID = tpguid;
							memcpy(&tpprt->Loc, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));
							tpprt->Loc.x += 5.0f*ConnUser[UserParsing].PointX;
							tpprt->Loc.y += 5.0f*ConnUser[UserParsing].PointY;

							try {
								if (NumParms < 4)
								{
									ServerMessage(UserParsing, "Error in !create portalloc syntax, check !help!", COLOR_RED);
									return;
								}
								char *usrep = strchr(Parms[1],'_');
								while (usrep)
								{
									*usrep = ' ';
									usrep = strchr(Parms[1],'_');
								}
								strcpy(tpprt->Name, Parms[1]);
								
								float NS, EW;//, Nx, Ny;
								char tpt;
								tpt = Parms[2][strlen(Parms[2])-1];
								Parms[2][strlen(Parms[2])-1] = 0;
								if ((tpt == 'N') || (tpt == 'n'))
								{
									sscanf(Parms[2],"%f",&NS);
								} else if ((tpt == 'S') || (tpt == 's')) {
									sscanf(Parms[2],"%f",&NS);
									NS *= -1;
								} else {
									NS = 0;
								}

								tpt = Parms[3][strlen(Parms[3])-1];
								Parms[3][strlen(Parms[3])-1] = 0;
								if ((tpt == 'E') || (tpt == 'e'))
								{
									sscanf(Parms[3],"%f",&EW);
								} else if ((tpt == 'W') || (tpt == 'w')) {
									sscanf(Parms[3],"%f",&EW);
									EW *= -1;
								} else {
									EW = 0;
								}

								tpprt->DestLoc.x = (EW*10.0f+1020.0f)*24.0f;
								tpprt->DestLoc.y = (NS*10.0f-1020.0f)*24.0f;
								if (NumParms == 5)
									sscanf(Parms[4], "%f", &tpprt->DestLoc.z);
								else
									tpprt->DestLoc.z = 0;
								
								tpprt->DestLoc.landblock = 
										( (BYTE) ((((DWORD) tpprt->DestLoc.x%192)/24)*8) + (((DWORD) tpprt->DestLoc.y%192)/24) ) |
										( (0x00) << 8) |
										( (BYTE) (tpprt->DestLoc.y/192.0f) << 16) |
										( (BYTE) (tpprt->DestLoc.x/192.0f) << 24)
									;
								tpprt->DestLoc.landblock -= 0x00010000;

								tpprt->DestLoc.x = (float) ((int) tpprt->DestLoc.x % 192);
								tpprt->DestLoc.y = (float) ((int) tpprt->DestLoc.y % 192);
							} catch (char *) {
								ServerMessage(UserParsing, "Error in !create portalloc syntax, check !help...", COLOR_RED);
								return;
							}

							LBObjects[tpprt->Loc.landblock >> 16][tpprt->GUID] = (cWorldObject *) tpprt;
							Portals[tpprt->Loc.landblock >> 16][tpprt->GUID] = tpprt;
							ObjectList[tpguid] = (cWorldObject *) tpprt;

							BYTE TpP[500];
							int packlen = tpprt->GenerateCreatePacket(TpP);
							for (int iii=0;iii<MaxUsers;iii++)
								if (ConnUser[iii].Connected)
									Send200(TpP, packlen, 3, iii);
						} else if (stricmp(Parms[0], "portal") == 0) {
							cWorldObject_Portal *tpprt = new cWorldObject_Portal();
							
							tpprt->GUID = tpguid;
							memcpy(&tpprt->Loc, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));
							tpprt->Loc.x += 3.0f;

							try {
								if (NumParms < 6)
								{
									ServerMessage(UserParsing, "Error in !create portal syntax, check !help!", COLOR_RED);
									return;
								}
								char *usrep = strchr(Parms[1],'_');
								while (usrep)
								{
									*usrep = ' ';
									usrep = strchr(Parms[1],'_');
								}
								strcpy(tpprt->Name, Parms[1]);
								sscanf(Parms[2],"%f",&tpprt->DestLoc.x);
								sscanf(Parms[3],"%f",&tpprt->DestLoc.y);
								sscanf(Parms[4],"%f",&tpprt->DestLoc.z);
								sscanf(Parms[5],"%08X",&tpprt->DestLoc.landblock);
							} catch (char *) {
								ServerMessage(UserParsing, "Error in !create portal syntax, check !help...", COLOR_RED);
								return;
							}

							LBObjects[tpprt->Loc.landblock >> 16][tpprt->GUID] = (cWorldObject *) tpprt;
							Portals[tpprt->Loc.landblock >> 16][tpprt->GUID] = tpprt;
							ObjectList[tpguid] = (cWorldObject *) tpprt;

							BYTE TpP[500];
							int packlen = tpprt->GenerateCreatePacket(TpP);
							for (int iii=0;iii<MaxUsers;iii++)
								if (ConnUser[iii].Connected)
									Send200(TpP, packlen, 3, iii);
						} else if (stricmp(Parms[0], "lifestone") == 0) {
							cWorldObject_LS *tpls = new cWorldObject_LS();
							
							tpls->GUID = tpguid;
							tpls->Type = OBJECT_LIFESTONE;
							memcpy(&tpls->Loc, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));
							tpls->Loc.x += 1.0f;

							LBObjects[tpls->Loc.landblock >> 16][tpls->GUID] = (cWorldObject *) tpls;
							ObjectList[tpguid] = (cWorldObject *) tpls;

							BYTE TpP[500];
							int packlen = tpls->GenerateCreatePacket(TpP);
							for (int iii=0;iii<MaxUsers;iii++)
								if (ConnUser[iii].Connected)
								Send200(TpP, packlen, 3, iii);
						} else if (stricmp(Parms[0], "obj") == 0) {
							if (stricmp(Parms[1], "help") == 0)
							{
								ServerMessage(UserParsing, "Syntax: !create obj -id ** [-name **] [-selectable]", COLOR_BLUE);
								ServerMessage(UserParsing, " * -id *** - Gives the object a Model # of ***\n\
 * -name *** - Names the object this\n\
 * -selectable - Makes the object selectable", COLOR_GREEN);
								return;
							}

							cWorldObject_ObjectID *tpObj = new cWorldObject_ObjectID();

							tpObj->Flags = 0;
							sprintf(tpObj->Name, "Unnamed Object");
							tpObj->Model = 1;

							try {
								int CurParm = 1;

								while (CurParm < NumParms)
								{
									if (stricmp(Parms[CurParm],"-id") == 0)
									{
										sscanf(Parms[CurParm+1], "%i", &tpObj->Model);
										CurParm++;
									}
									else if (stricmp(Parms[CurParm],"-icon") == 0)
									{
										sscanf(Parms[CurParm+1], "%i", &tpObj->Icon);
										CurParm++;
									}
									else if (stricmp(Parms[CurParm],"-scale") == 0)
									{
										sscanf(Parms[CurParm+1], "%f", &tpObj->Scale);
										CurParm++;
									}
									else if (stricmp(Parms[CurParm],"-selectable") == 0)
									{
										tpObj->Flags |= BYOBJ_SELECTABLE;
									}
									else if (stricmp(Parms[CurParm],"-name") == 0)
									{
										sscanf(Parms[CurParm+1], "%s", &tpObj->Name);
										char *usrep = strchr(tpObj->Name,'_');
										while (usrep)
										{
											*usrep = ' ';
											usrep = strchr(tpObj->Name,'_');
										}
										CurParm++;
									}
									CurParm++;
								}
							} catch (...) {
								sprintf(sayit, "Error, check syntax: %s", messagetext);
								ServerMessage(UserParsing, sayit, COLOR_RED);
							}

							sprintf(sayit, "Model Created with ID: %i", tpObj->Model);
							ServerMessage(UserParsing, sayit, COLOR_RED);

							tpObj->GUID = tpguid;
							tpObj->Type = OBJECT_OBJECTID;
							memcpy(&tpObj->Loc, &ConnUser[UserParsing].Char->Loc, sizeof(Location_t));
							tpObj->Loc.x += 5.0f*ConnUser[UserParsing].PointX;
							tpObj->Loc.y += 5.0f*ConnUser[UserParsing].PointY;

							LBObjects[tpObj->Loc.landblock >> 16][tpObj->GUID] = (cWorldObject *) tpObj;
							ObjectList[tpguid] = (cWorldObject *) tpObj;

							BYTE TpP[500];
							int packlen = tpObj->GenerateCreatePacket(TpP);
							for (int iii=0;iii<MaxUsers;iii++)
								if (ConnUser[iii].Connected)
									Send200(TpP, packlen, 3, iii);
						}
					} else if (stricmp(&command[1],"renamesel") == 0) {
						DWORD GUIDtoChange = ConnUser[UserParsing].SelectedItem;
						cWorldObject *newobj = ObjectList[GUIDtoChange];
						if (newobj)
						{
							sprintf(newobj->Name, "%s", Parms[0]);
							sprintf(sayit, "Object %08X renamed to %s!", GUIDtoChange, newobj->Name);
							ServerMessage(UserParsing, sayit, COLOR_BLUE);
						}
					} else if (stricmp(&command[1],"renameobject") == 0) {
						DWORD GUIDtoChange; sscanf(Parms[0], "%08x", &GUIDtoChange);
						cWorldObject *newobj = ObjectList[GUIDtoChange];
						if (newobj)
						{
							sprintf(newobj->Name, "%s", Parms[1]);
							sprintf(sayit, "Object %08X renamed to %s!", GUIDtoChange, newobj->Name);
							ServerMessage(UserParsing, sayit, COLOR_BLUE);
						}
					} else if (stricmp(&command[1],"listobjects") == 0) {
						ServerMessage(UserParsing, "Listing Objects in Current Landblock...", COLOR_BLUE);
						for (std::map<DWORD, cWorldObject *>::iterator i = LBObjects[ConnUser[UserParsing].Char->Loc.landblock >> 16].begin(); i != LBObjects[ConnUser[UserParsing].Char->Loc.landblock >> 16].end(); i++)
						{
							if ((i->second->Type) && (i->second->GUID != ConnUser[UserParsing].Char->GUID))
							{
								sprintf(sayit, "Object: %08X, Type: %i, Name: %s", i->second->GUID, i->second->Type, i->second->Name);
								ServerMessage(UserParsing, sayit, COLOR_BLUE);
							}
						}
						ServerMessage(UserParsing, "End of list!", COLOR_BLUE);
					} else if (stricmp(&command[1],"anim") == 0) {
						try {
							DWORD tpdw;
							sscanf(Contents, "%04X",  &tpdw);
							
							SendAnim(UserParsing, tpdw, 1.0f);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !anim syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"particle") == 0) {
						try {
							DWORD tpdw;
							sscanf(Contents,"%04x",&tpdw);

							SendParticle(UserParsing, tpdw, 1.0f);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !particle syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"sound") == 0) {
						try {
							DWORD tpdw;
							sscanf(Contents,"%04x",&tpdw);

							SendSound(UserParsing, tpdw, 1.0f);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !particle syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"smite") == 0) {
						try {
							DWORD Playertokill;
							sscanf(Contents, "%04X",  &Playertokill);
							if (Playertokill > (DWORD) MaxUsers)
								return;

							if (!ConnUser[Playertokill].Char)
								return;

							SendAnim(Playertokill, 0x08, 1.0f);

							sprintf(sayit, "%s smites %s!", ConnUser[UserParsing].Char->Name, ConnUser[Playertokill].Char->Name);
							ServerMessage(GLOBAL_MESSAGE, sayit, COLOR_GREEN);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !smite syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"summon") == 0) {
						DWORD ToAssist;
						try {
							sscanf(Contents, "%i", &ToAssist);

							if ((ToAssist > 128) || (!ConnUser[ToAssist].Connected) || (ConnUser[ToAssist].State < 5))
							{
								ServerMessage(UserParsing, "Invalid User...", COLOR_RED);	
								return;
							}
							
							MoveUser(ToAssist, &ConnUser[UserParsing].Char->Loc, true, false);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !summon syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"goto") == 0) {
						DWORD ToGoto;
						try {
							sscanf(Contents, "%i", &ToGoto);
							if ((ToGoto > 128) || (!ConnUser[ToGoto].Connected) || (ConnUser[ToGoto].State < 5))
							{
								ServerMessage(UserParsing, "Invalid User...", COLOR_RED);	
								return;
							}
							
							MoveUser(UserParsing, &ConnUser[ToGoto].Char->Loc, true, false);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !goto syntax, check !help...", COLOR_RED);
							return;
						}
					} else if (stricmp(&command[1],"telemapother") == 0) {
						try {
							if (NumParms < 3)
							{
								ServerMessage(UserParsing, "Error in !telemap syntax, check !help!", COLOR_RED);
								return;
							}

							DWORD Chartotele;
							sscanf(Parms[0],"%i",&Chartotele);
							
							float NS, EW;//, Nx, Ny;
							char tpt;
							tpt = Parms[0][strlen(Parms[1])-1];
							Parms[1][strlen(Parms[1])-1] = 0;
							if ((tpt == 'N') || (tpt == 'n'))
							{
								sscanf(Parms[1],"%f",&NS);
							} else if ((tpt == 'S') || (tpt == 's')) {
								sscanf(Parms[1],"%f",&NS);
								NS *= -1;
							} else {
								NS = 0;
							}

							tpt = Parms[2][strlen(Parms[2])-1];
							Parms[2][strlen(Parms[2])-1] = 0;
							if ((tpt == 'E') || (tpt == 'e'))
							{
								sscanf(Parms[2],"%f",&EW);
							} else if ((tpt == 'W') || (tpt == 'w')) {
								sscanf(Parms[2],"%f",&EW);
								EW *= -1;
							} else {
								EW = 0;
							}

							Location_t templ;
							
							templ.x = (EW*10.0f+1020.0f)*24.0f;
							templ.y = (NS*10.0f-1020.0f)*24.0f;
							if (NumParms == 4)
								sscanf(Parms[3], "%f", &templ.z);
							else
								templ.z = 500.0f;

							templ.landblock = 
									( (BYTE) ((((DWORD) templ.x%192)/24)*8) + (((DWORD) templ.y%192)/24) ) |
									( (0x00) << 8) |
									( (BYTE) (templ.y/192.0f) << 16) |
									( (BYTE) (templ.x/192.0f) << 24)
								;
							templ.landblock -= 0x00010000;

							templ.x = (float) ((int) templ.x % 192);
							templ.y = (float) ((int) templ.y % 192);

							MoveUser(Chartotele, &templ, true, false);
						} catch (char *) {
							ServerMessage(UserParsing, "Error in !telemapother syntax, check !help...", COLOR_RED);
							return;
						}
					}
				} else {
					//Actual text message... omgomgmog
					BYTE tpmessage[0x1000];	ZeroMemory(tpmessage, sizeof(tpmessage));
					BYTE * PacketPointer = &tpmessage[0];
						
					int tplen;
					DWORD tpn = 0x00000037; memcpy(PacketPointer, &tpn, 4); PacketPointer += 4;
					GenString(messagetext, &tplen, PacketPointer);	PacketPointer += tplen;
					GenString(&ConnUser[UserParsing].Char->Name[0], &tplen, PacketPointer);	PacketPointer += tplen;
										memcpy(PacketPointer, &ConnUser[UserParsing].Char->GUID, 4); PacketPointer += 4;
					tpn = 0x2;			memcpy(PacketPointer, &tpn, 4); PacketPointer += 4;
					
					for (int iii=0;iii<MaxUsers;iii++)
					{
						if ((ConnUser[iii].Connected) && (ConnUser[iii].State == 6))
							Send200(&tpmessage[0], (int) (PacketPointer - &tpmessage[0]), 4, iii);
					}
				}
//				delete [] messagetext;
			}
		}
		break;
	case 0x01DF:
		{
			//Emote!
			char *messagetext = new char[PacketLen-2];
			memcpy(messagetext,Packet + 2,PacketLen-2);

			BYTE tpmessage[0x1000];	ZeroMemory(tpmessage, sizeof(tpmessage));
			BYTE * PacketPointer = &tpmessage[0];
				
			int tplen;
			DWORD tpn = 0x000001E2;
			memcpy(PacketPointer, &tpn, 4); PacketPointer += 4;
			memcpy(PacketPointer, &ConnUser[UserParsing].Char->GUID, 4); PacketPointer += 4;
			GenString(&ConnUser[UserParsing].Char->Name[0], &tplen, PacketPointer);	PacketPointer += tplen;
			GenString(messagetext, &tplen, PacketPointer);	PacketPointer += tplen;

			for (int iii=0;iii<MaxUsers;iii++)
			{
				if ((ConnUser[iii].Connected) && (ConnUser[iii].State == 6))
					Send200(&tpmessage[0], (int) (PacketPointer - &tpmessage[0]), 4, iii);
			}
		}
		break;
	case 0x01E1:
		//Emote Text...
		char MsgText[100];
		sprintf(MsgText, "%s %s", ConnUser[UserParsing].Char->Name, Packet+2);

		ServerMessage(-1*(UserParsing+1), &MsgText[0], COLOR_GREY);
		break;
	case 0x0053:
		{
			//Toggle Combat Mode
			//4th dword: 1 = peace, 2 = war
		
			DWORD CombatMode; memcpy(&CombatMode, Packet, 4);
			if (CombatMode == 1)
			{
				SendAnim(UserParsing, 0x3b, 1.0f);
			} else {
				if (ConnUser[UserParsing].Char->Equipped[EQUIPPED_WEAPON] == 0)
				{
					//Punching
					SendAnim(UserParsing, 0x3c, 1.0f);
				} else {
		//				cWorldObject_Weapon *WeaponUsing = (cWorldObject_Weapon *) ConnUser[UserParsing].ObjectCache[ConnUser[UserParsing].Char->Equipped[EQUIPPED_WEAPON]];
		//				if (!WeaponUsing)
		//					WeaponUsing = (cWorldObject_Weapon *) ObjectList[ConnUser[UserParsing].Char->Equipped[EQUIPPED_WEAPON]];
				}
			}
		}
		break;
	case 0x021E:
		//Dunno
		break;
	case 0x00C8:
		{
			//Eval
			//4th dword = GUID to eval

			DWORD dwEvalGUID; memcpy(&dwEvalGUID, Packet, 4);

			EvalObject(UserParsing, dwEvalGUID);
		}
		break;
	case 0x0019:
		{
			//Move Item
			//4th dword = GUID to move
			//5th dword = GUID of item to move it into... Player for equipping, bag for putting it in.
			//6th dword = Index in item to put it in

			DWORD GUIDMove;
			memcpy(&GUIDMove, Packet,4);

			//F7B0/22
			DWORD MoveItemArr[] = {
				0x0000F7B0,
				ConnUser[UserParsing].Char->GUID,
				ConnUser[UserParsing].EventCount,
				0x00000022,
				0,
				0,
				0
			};
			memcpy(&MoveItemArr[4],Packet,3*4);
			ConnUser[UserParsing].EventCount++;
			Send200((BYTE *) MoveItemArr, sizeof(MoveItemArr), 4, UserParsing);

			cWorldObject *tpo = ObjectList[GUIDMove];
			if (!tpo->Owner)
			{
				DWORD MoveItemCont[] = {
					0x0000F74A,
					GUIDMove
				};
				Send200((BYTE *) MoveItemCont, 8, 4, UserParsing);
			}

			tpo->Owner = MoveItemArr[5];

			if (GUIDMove == ConnUser[UserParsing].Char->Equipped[EQUIPPED_WEAPON])
			{
				//First 22D
				BYTE Pack22D[0x15]; ZeroMemory(Pack22D, 0x15);
				DWORD tpdword;	BYTE tpbyte;
				tpdword = 0x22D; memcpy(&Pack22D[0], &tpdword, 4);		//22D = set wielder/container
				tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack22D[4], &tpbyte, 1);	//EquipCount
				//byte here...
								 memcpy(&Pack22D[5], &GUIDMove, 4);		//Item
				tpdword = 3;     memcpy(&Pack22D[9], &tpdword, 4);		//Set Wielder
				tpdword = 0;     memcpy(&Pack22D[13], &tpdword, 4);		//Wielder = 0
				tpdword = ConnUser[UserParsing].EquipCount + 1;     memcpy(&Pack22D[17], &tpdword, 4);		//unknown2...
				Send200(Pack22D, sizeof(Pack22D), 4, UserParsing);

				//229
				BYTE Pack229[0x11]; ZeroMemory(Pack229, 0x11);
				tpdword = 0x229; memcpy(&Pack229[0], &tpdword, 4);		//229 = set somin
				tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack229[4], &tpbyte, 1);	//EquipCount
				//byte here...
								 memcpy(&Pack229[5], &GUIDMove, 4);		//Item
				tpdword = 0xA;   memcpy(&Pack229[9], &tpdword, 4);		//Unknown, always A...
				tpdword = 0;     memcpy(&Pack229[13], &tpdword, 4);		//Coverage = 0
				Send200(Pack229, sizeof(Pack229), 4, UserParsing);

				//Second 22D
				ZeroMemory(Pack22D, 0x15);
				tpdword = 0x22D; memcpy(&Pack22D[0], &tpdword, 4);		//22D = set wielder/container
				tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack22D[4], &tpbyte, 1);	//EquipCount
				//byte here...
								 memcpy(&Pack22D[5], &GUIDMove, 4);		//Item
				tpdword = 2;     memcpy(&Pack22D[9], &tpdword, 4);		//Set Container
								 memcpy(&Pack22D[13], Packet + 4, 4);		//Container = Bag
				tpdword = 0x34500;     memcpy(&Pack22D[17], &tpdword, 4);		//unknown2...
				Send200(Pack22D, sizeof(Pack22D), 4, UserParsing);
				
				//EQ++
				ConnUser[UserParsing].EquipCount ++;
			} else {
				BYTE Pack22D[0x15]; ZeroMemory(Pack22D, 0x15);
				DWORD tpdword;	BYTE tpbyte;
				tpdword = 0x22D; memcpy(&Pack22D[0], &tpdword, 4);		//22D = set wielder/container
				tpbyte = ConnUser[UserParsing].MoveItemCount;	memcpy(&Pack22D[4], &tpbyte, 1);	//MoveItemCount
				//byte here...
								 memcpy(&Pack22D[5], &GUIDMove, 4);		//Item
				tpdword = 2;     memcpy(&Pack22D[9], &tpdword, 4);		//2 = Set Container
								 memcpy(&Pack22D[13], Packet + 4, 4);	//Container (pack)
				tpdword = 0x34500;     memcpy(&Pack22D[17], &tpdword, 4);		//unknown2
				Send200(Pack22D, sizeof(Pack22D), 4, UserParsing);
				ConnUser[UserParsing].MoveItemCount += 2;
			}
		}
		break;
	case 0x001A:
		{
			//Item Worn
			//4th dword = GUID of item
			//5th dword = coverage

			//We'll just go with equipped items now, m'kay? m'kay.

			DWORD GUIDEquip, Coverage;
			memcpy(&GUIDEquip, Packet, 4);
			memcpy(&Coverage, Packet + 4, 4);		//Ignore coverage for now m'kay? m'kay.

			//F7B0/23
			DWORD F7B023[] = {
				0x0000F7B0,
				ConnUser[UserParsing].Char->GUID,
				ConnUser[UserParsing].EventCount,
				0x00000023,
				GUIDEquip,
				Coverage
			};
			ConnUser[UserParsing].EventCount++;
			Send200((BYTE *) &F7B023[0], sizeof(F7B023), 4, UserParsing);
			
			//First 22D
			BYTE Pack22D[0x15]; ZeroMemory(Pack22D, 0x15);
			DWORD tpdword;	BYTE tpbyte;
			tpdword = 0x22D; memcpy(&Pack22D[0], &tpdword, 4);		//22D = set wielder/container
			tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack22D[4], &tpbyte, 1);	//EquipCount
			//byte here...
							 memcpy(&Pack22D[5], &GUIDEquip, 4);		//Item
			tpdword = 3;     memcpy(&Pack22D[9], &tpdword, 4);		//Set Wielder
							 memcpy(&Pack22D[13], &ConnUser[UserParsing].Char->GUID, 4);		//Wielder = Player
			tpdword = ConnUser[UserParsing].EquipCount + 1;     memcpy(&Pack22D[17], &tpdword, 4);		//EquipCount + 1
			Send200(Pack22D, sizeof(Pack22D), 4, UserParsing);
			
			//Second 22D
			ZeroMemory(Pack22D, 0x15);
			tpdword = 0x22D; memcpy(&Pack22D[0], &tpdword, 4);		//22D = set wielder/container
			tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack22D[4], &tpbyte, 1);	//EquipCount
			//byte here...
							 memcpy(&Pack22D[5], &GUIDEquip, 4);		//Item
			tpdword = 2;     memcpy(&Pack22D[9], &tpdword, 4);		//Set Container
			tpdword = 0;     memcpy(&Pack22D[13], &tpdword, 4);		//Container = 0
			tpdword = ConnUser[UserParsing].EquipCount + 1;     memcpy(&Pack22D[17], &tpdword, 4);		//EquipCount + 1
			Send200(Pack22D, sizeof(Pack22D), 4, UserParsing);

			//229
			BYTE Pack229[0x11]; ZeroMemory(Pack229, 0x11);
			tpdword = 0x229; memcpy(&Pack229[0], &tpdword, 4);		//22D = set wielder/container
			tpbyte = ConnUser[UserParsing].EquipCount;	memcpy(&Pack229[4], &tpbyte, 1);	//EquipCount
			//byte here...
							 memcpy(&Pack229[5], &GUIDEquip, 4);		//Item
			tpdword = 0xA;   memcpy(&Pack229[9], &tpdword, 4);		//Unknown, always A...
							 memcpy(&Pack229[13], &Coverage, 4);		//Coverage = 0
			Send200(Pack229, sizeof(Pack229), 4, UserParsing);
			
			//F749
			DWORD F749[] = {
				ConnUser[UserParsing].Char->GUID,	//Wielder
				GUIDEquip,						//Item
				1,								//Unk1
				1,								//Unk2
				//40254,							//Unk3
				0x00254 | (0x40000 + 0x30000*((ConnUser[UserParsing].EquipCount - 1) / 2)),	//Unk3, try this...
				0x80000000						//Unk4
			};
			Send200((BYTE *) &F749[0], sizeof(F749), 4, UserParsing);

			//EQ++
			ConnUser[UserParsing].EquipCount ++;

			ConnUser[UserParsing].Char->Equipped[EQUIPPED_WEAPON] = GUIDEquip;
		}
		break;
	case 0x001B:
		//Item Dropped on Ground
		//4th dword = GUID dropped
		
		//?
		break;
	case 0x0263:
		//Item Selected
		//4th dword = GUID selected
		
		memcpy(&ConnUser[UserParsing].SelectedItem, Packet,4);
		break;
	case 0x01bf:
		//Item Selected
		//4th dword = GUID selected
		
		memcpy(&ConnUser[UserParsing].SelectedItem, Packet,4);
		break;
	case 0x01B7:
		//Beginning of Jump
		break;
	case 0xF61B:
		//Player Jumped
		break;
	case 0x00CA:
		//Dunno
		break;
	case 0x01A1:
		//Dunno
		break;
	default:
		{
			char *tps2, *tps3 = new char[1 + PacketLen*5];
			ZeroMemory(tps3,1 + PacketLen*5);
			tps2 = tps3;
			for (unsigned int tpl=0; tpl < PacketLen; tpl++)
			{
				sprintf(tps2, "0x%02X ", Packet[tpl]);
				tps2+=5;
			}
			sprintf(sayit, "F7B1/%04X Pack Data: %s", MessageType, tps3); LogDisp(sayit);
			delete [] tps3;
			break;
		}
	}
}

