#include "stdafx.h"
#include "HighLevelFuncs.h"

void cACServer::ActionComplete(int User)
{
	DWORD ActComp[] = {
		0x0000F7B0,
		ConnUser[User].Char->GUID,
		ConnUser[User].EventCount,
		0x000001C7,
		0x00000000,
		0x00000000
	};
	ConnUser[User].EventCount++;
	Send200((BYTE *) ActComp, sizeof(ActComp), 3, User);
}

void cACServer::MoveUser(int User, Location_t *loc, bool Portal, bool Override)
{
	BYTE MOVE_ENTITY[] = {
		0x48, 0xF7, 0x00, 0x00, 0x03, 0x40, 0x16, 0x78, 0x34, 0x00, 0x00, 0x00, 0x08, 0x01, 0x64, 0x81,   //(H÷___@_x4_____d_) - 0000
		0xA1, 0xDB, 0x18, 0x41, 0xF0, 0x07, 0x19, 0x43, 0xD7, 0xA3, 0xBD, 0x41, 0xAA, 0x77, 0x6D, 0x3F,   //(___A___C___A_wm?) - 0010
		0xE0, 0x42, 0xBF, 0xBE, 0x39, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,                           //(_B__9_______)     - 0020
	};

	for (std::map<DWORD, cWorldObject *>::iterator i = Portals[loc->landblock >> 16].begin(); i != Portals[loc->landblock >> 16].end(); i++)
	{
		if (GetDistance(loc, &i->second->Loc) < 0.03)
		{
			MoveUser(User, &((cWorldObject_Portal *)i->second)->DestLoc, true, false);
//			ServerMessage(User, "Entered Portal... Teleporting...", COLOR_RED);
		}
	}

	float heading = acos(loc->a);
	if (loc->w > 0) heading = PI - heading;
	heading *= 2;
	ConnUser[User].PointX = sin(heading);
	ConnUser[User].PointY = cos(heading);

	if (!ConnUser[User].Char)
		return;

	WORD OldLB = ConnUser[User].Char->Loc.landblock >> 16, NewLB = loc->landblock >> 16;
	
	memcpy(&ConnUser[User].Char->Loc, loc, sizeof(Location_t));

	DWORD tpf2 = 0x0000F748; memcpy (&MOVE_ENTITY[0x00],&tpf2,4);
	memcpy(&MOVE_ENTITY[0x4], &ConnUser[User].Char->GUID, 4);
	tpf2 = 0x00000034; memcpy (&MOVE_ENTITY[0x08],&tpf2,4);
	memcpy(&MOVE_ENTITY[0x0C], &ConnUser[User].Char->Loc, 4*5);
	memcpy(&MOVE_ENTITY[0x20], &ConnUser[User].Char->Loc.w, 4);

	if (Portal)
		ConnUser[User].PortalCount++;
	if (Override)
		ConnUser[User].OverrideCount++;
	
	memcpy (&MOVE_ENTITY[0x24],&ConnUser[User].Char->NumLogins,2);
	memcpy (&MOVE_ENTITY[0x26],&ConnUser[User].move_count,2);
	memcpy (&MOVE_ENTITY[0x28],&ConnUser[User].PortalCount,2);
	memcpy (&MOVE_ENTITY[0x2A],&ConnUser[User].OverrideCount,2);

	for (int iii=0;iii<MaxUsers;iii++)
	{
		if ((ConnUser[iii].Connected) && ((ConnUser[iii].State == 6) || (iii == User)))
			Send200(&MOVE_ENTITY[0], sizeof(MOVE_ENTITY), 3, iii);
	}
	ConnUser[User].move_count++;

	if (OldLB != NewLB)
	{
		
		LBObjects[OldLB].erase(ConnUser[User].Char->GUID);
		LBObjects[NewLB][ConnUser[User].Char->GUID] = (cWorldObject *) ConnUser[User].Char;


		if (OldLB == NewLB - 1) {
			SendLandblock(User, NewLB - 1);
			SendLandblock(User, NewLB - 1 - 256);
			SendLandblock(User, NewLB - 1 + 256);
		} else if (OldLB == NewLB + 1) {
			SendLandblock(User, NewLB + 1);
			SendLandblock(User, NewLB + 1 - 256);
			SendLandblock(User, NewLB + 1 + 256);
		} else if (OldLB == NewLB - 256) {
			SendLandblock(User, NewLB - 256 - 1);
			SendLandblock(User, NewLB - 256);
			SendLandblock(User, NewLB - 256 + 1);
		} else if (OldLB == NewLB + 256) {
			SendLandblock(User, NewLB + 256 - 1);
			SendLandblock(User, NewLB + 256);
			SendLandblock(User, NewLB + 256 + 1);
//Diagonals Now
		} else if (OldLB == NewLB + 256 + 1) {
			SendLandblock(User, NewLB + 256 + 1);
			SendLandblock(User, NewLB + 256 + 1 - 256);
			SendLandblock(User, NewLB + 256 + 1 - 256 - 256 );
			SendLandblock(User, NewLB + 256 + 1 - 1);
			SendLandblock(User, NewLB + 256 + 1 - 1 - 1);
		} else if (OldLB == NewLB + 256 - 1) {
			SendLandblock(User, NewLB + 256 - 1);
			SendLandblock(User, NewLB + 256 - 1 - 256);
			SendLandblock(User, NewLB + 256 - 1 - 256 - 256 );
			SendLandblock(User, NewLB + 256 - 1 + 1);
			SendLandblock(User, NewLB + 256 - 1 + 1 + 1);
		} else if (OldLB == NewLB - 256 + 1) {
			SendLandblock(User, NewLB - 256 + 1);
			SendLandblock(User, NewLB - 256 + 1 + 256);
			SendLandblock(User, NewLB - 256 + 1 + 256 + 256 );
			SendLandblock(User, NewLB - 256 + 1 - 1);
			SendLandblock(User, NewLB - 256 + 1 - 1 - 1);
		} else if (OldLB == NewLB - 256 - 1) {
			SendLandblock(User, NewLB - 256 - 1);
			SendLandblock(User, NewLB - 256 - 1 + 256);
			SendLandblock(User, NewLB - 256 - 1 + 256 + 256 );
			SendLandblock(User, NewLB - 256 - 1 + 1);
			SendLandblock(User, NewLB - 256 - 1 + 1 + 1);
		} else {
			SendLandblock(User, NewLB);
			SendLandblock(User, NewLB - 1);
			SendLandblock(User, NewLB + 1);
			SendLandblock(User, NewLB - 256);
			SendLandblock(User, NewLB - 256 - 1);
			SendLandblock(User, NewLB - 256 + 1);
			SendLandblock(User, NewLB + 256);
			SendLandblock(User, NewLB + 256 - 1);
			SendLandblock(User, NewLB + 256 + 1);
		}
	}
}

void cACServer::SendAnim(int User, WORD AnimNum, float PlaySpeed)
{
	ConnUser[User].AnimCount++;

	cAnimStruct ASt;
	ASt.dwf74c = 0x0000F74C;
	ASt.dwGUID = ConnUser[User].Char->GUID;
	ASt.wNumLogins = ConnUser[User].Char->NumLogins;
	ASt.wNumInteractions = ConnUser[User].move_count;
	ASt.wNumAnimations = ConnUser[User].AnimCount;
	ASt.wFlag = 0;
	ASt.wUnk1 = 0;
	ASt.wAnimation_Family = 0x3D;
	ASt.wUnk2 = 0x80;
	ASt.wUnk3 = 0;
	ASt.wAnimation_Seqnum = ConnUser[User].AnimCount;
	ASt.wAnimation_to_Play = (WORD) AnimNum;
	ASt.fPlaySpeed = PlaySpeed;

	ConnUser[User].move_count++;

	for (int i=0;i<MaxUsers;i++)
	{
		if (ConnUser[i].Connected)
			Send200((BYTE *) &ASt, sizeof(ASt), 3, i);
	}
}

void cACServer::PointAt(int User, DWORD GUIDAt)
{
	ConnUser[User].AnimCount++;

	cAnimStructEx ASt;
	ASt.dwf74c = 0x0000F74C;
	ASt.dwGUID = ConnUser[User].Char->GUID;
	ASt.wNumLogins = ConnUser[User].Char->NumLogins;
	ASt.wNumInteractions = ConnUser[User].move_count;
	ASt.wNumAnimations = ConnUser[User].AnimCount;
	ASt.wFlag = 0;
	ASt.wUnk1 = 0;
	ASt.wAnimation_Family = 0x3D;

	ASt.dwf74c = 0x0000F74C;
	ASt.dwGUID = ConnUser[User].Char->GUID;
	ASt.wNumLogins = ConnUser[User].Char->NumLogins;
	ASt.wNumInteractions = ConnUser[User].move_count;
	ASt.wNumAnimations = ConnUser[User].AnimCount;
	ASt.wFlag = 0;
	ASt.wUnk1 = 0x0008;
	ASt.wAnimation_Family = 0x3D;
	ASt.GUIDTarget = GUIDAt;
	ASt.dwUnk1 = 0x429DB3C7;
	ASt.dwUnk2 = 0x0155EE0F;
	ASt.fUnk1 = 1.0f;
	ASt.Zero = 0;

	ConnUser[User].move_count++;

	for (int i=0;i<MaxUsers;i++)
	{
		if (ConnUser[i].Connected)
			Send200((BYTE *) &ASt, sizeof(ASt), 3, i);
	}

}

void cACServer::SendParticle(int User, DWORD ParticleNum, float PlaySpeed)
{
	DWORD ParticleEffect[] = {
		0xF755,
		ConnUser[User].Char->GUID,
		ParticleNum,
		0
	};

	memcpy(&ParticleEffect[3],&PlaySpeed,4);

	ConnUser[User].move_count++;

	for (int i=0;i<MaxUsers;i++)
	{
		if (ConnUser[i].Connected)
			Send200((BYTE *) &ParticleEffect, sizeof(ParticleEffect), 3, i);
	}
}

void cACServer::SendSound(int User, DWORD SoundNum, float PlaySpeed)
{
	DWORD SoundEffect[] = {
		0xF750,
		ConnUser[User].Char->GUID,
		SoundNum,
		0
	};

	memcpy(&SoundEffect[3],&PlaySpeed,4);

	ConnUser[User].move_count++;

	for (int i=0;i<MaxUsers;i++)
	{
		if (ConnUser[i].Connected)
			Send200((BYTE *) &SoundEffect, sizeof(SoundEffect), 3, i);
	}
}

void cACServer::GiveXP(int User, int Type, DWORD Amount)
{
	BYTE Pack237[0xD];
	DWORD tpdword;	BYTE tpbyte;

	if ((User > 128) || (!ConnUser[User].Connected) || (ConnUser[User].State != 6))
	{
		ServerMessage(User, "Error in !givexp format... Make sure you're giving a valid User #...", COLOR_RED);
		
		return;
	}

	DWORD OldXP = ConnUser[User].Char->TotalXP;
	ConnUser[User].Char->TotalXP += Amount;
	if (ConnUser[User].Char->TotalXP < OldXP)
		ConnUser[User].Char->TotalXP = LevelArray[126] + 5000;
	if (ConnUser[User].Char->TotalXP > LevelArray[126])
		ConnUser[User].Char->TotalXP = LevelArray[126] + 5000;

	ZeroMemory(Pack237, 0xD);
	tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
	tpbyte = ConnUser[User].Count237[0x15];	memcpy(&Pack237[4], &tpbyte, 1);
	tpdword = 0x15;	 memcpy(&Pack237[5], &tpdword, 4);		//Total XP
	tpdword = ConnUser[User].Char->TotalXP;  memcpy(&Pack237[9], &tpdword, 4);
	Send200(Pack237, sizeof(Pack237), 4, User);
	ConnUser[User].Count237[0x15]++;

	int NewLevel = 0;
	for (int i=ConnUser[User].Char->Level;i<128;i++)
	{
		if (ConnUser[User].Char->TotalXP < LevelArray[i])
		{
			NewLevel = i - 1;

			i = 128;
		}
	}
	if (NewLevel == 0)
		NewLevel = 126;

	bool LL = false;
	if (NewLevel > ConnUser[User].Char->Level)
	{
		//Level him!
		SendParticle(User, 0x89, 1.0f);
		
		//Give skill credits as appropriate...
		for (i=ConnUser[User].Char->Level+1;i<=NewLevel;i++)
		{
			for (int h=0;h<=sizeof(CreditLevel)/4;h++)
				if (CreditLevel[h] == i)
					ConnUser[User].Char->SkillCredits++;
		}
		
		ConnUser[User].Char->Level = NewLevel;
		
		ZeroMemory(Pack237, 0xD);
		tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
		tpbyte = ConnUser[User].Count237[0x19];	memcpy(&Pack237[4], &tpbyte, 1);
		tpdword = 0x19;	 memcpy(&Pack237[5], &tpdword, 4);		//Level
		tpdword = ConnUser[User].Char->Level;  memcpy(&Pack237[9], &tpdword, 4);
		Send200(Pack237, sizeof(Pack237), 4, User);
		ConnUser[User].Count237[0x19]++;

		ZeroMemory(Pack237, 0xD);
		tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
		tpbyte = ConnUser[User].Count237[0x18];	memcpy(&Pack237[4], &tpbyte, 1);
		tpdword = 0x18;	 memcpy(&Pack237[5], &tpdword, 4);		//Credits
		tpdword = ConnUser[User].Char->SkillCredits;  memcpy(&Pack237[9], &tpdword, 4);
		Send200(Pack237, sizeof(Pack237), 4, User);
		ConnUser[User].Count237[0x18]++;

		LL = true;
	}

	if ((Type < 0) || ((Type > 39) && (Type != 128)))
	{
		ServerMessage(User, "Error in !givexp format... Type should be from 0-39 or 128 for unassigned XP...", COLOR_RED);

		return;
	}

	if (Type == SKILL_UNASSIGNED)
	{
		ConnUser[User].Char->UnassignedXP += Amount;
		
		ZeroMemory(Pack237, 0xD);
		tpdword = 0x237; memcpy(&Pack237[0], &tpdword, 4);
		tpbyte = ConnUser[User].Count237[0x16];	memcpy(&Pack237[4], &tpbyte, 1);
		tpdword = 0x16;	 memcpy(&Pack237[5], &tpdword, 4);		//Unassigned XP
		tpdword = ConnUser[User].Char->UnassignedXP;  memcpy(&Pack237[9], &tpdword, 4);
		Send200(Pack237, sizeof(Pack237), 4, User);
		ConnUser[User].Count237[0x16]++;
	} else if (Type < 40) {
		//Differetiate between train/spec eventually
		
		ConnUser[User].Char->SkillXP[Type] += Amount;

		int NewSkillLevel = 0;
		for (int i=0;i<200;i++)
		{
			if (ConnUser[User].Char->SkillXP[Type] < TrainedSkillArray[i])
			{
				NewSkillLevel = i - 1;

				i = 200;
			}
		}
		if (NewSkillLevel > ConnUser[User].Char->SkillInc[Type])
		{
			ConnUser[User].Char->SkillInc[Type] = NewSkillLevel;
		}

		//Send skill xp packet
	}

	if (!LL)
		return;

	sprintf(sayit, "You are now level %i!", ConnUser[User].Char->Level);
	ServerMessage(User, sayit, COLOR_CYAN);
	char mults[2] = { 's', 0 };
	if (ConnUser[User].Char->SkillCredits == 1)
		mults[0] = 0;
	sprintf(sayit, "You have %u experience points and %i skill credit%s available to raise skills and attributes.", ConnUser[User].Char->UnassignedXP, ConnUser[User].Char->SkillCredits, mults);
	ServerMessage(User, sayit, COLOR_CYAN);
	int NextCredit = 0;
	i = 0;
	while (!NextCredit)
	{
		if (ConnUser[User].Char->Level < CreditLevel[i])
			NextCredit = CreditLevel[i];
		
		i++;
	}
	if (NextCredit == 666)
	{
		sprintf(sayit, "Sorry, you will never earn another skill credit with levels...");
	} else {
		sprintf(sayit, "You will earn another skill credit at level %i.", NextCredit);
	}
	ServerMessage(User, sayit, COLOR_CYAN);
}

void cACServer::RemoveItem(DWORD GUID)
{
	DWORD F747[] = {
		0x0000F747,
		GUID,
		0x5
	};
	for (int i=0;i<MaxUsers;i++)
	{
		if ((ConnUser[i].Connected) && (ConnUser[i].State == 6)) {
			Send200((BYTE *) F747, sizeof(F747), 3, i);
		}
	}
}

void cACServer::EvalObject(int User, DWORD GUID)
{
	cWorldObject *cwoItem = ObjectList[GUID];

	if (!cwoItem)
		return;

	BYTE OutPack[0x500];
	BYTE *PackPointer = OutPack;
	DWORD tpd;//	WORD tpw;
	
	//F7b0 header
	tpd = 0xF7B0;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
						memcpy(PackPointer, &ConnUser[User].Char->GUID, 4);	PackPointer += 4;
						memcpy(PackPointer, &ConnUser[User].EventCount, 4);	PackPointer += 4;
	tpd = 0x00C9;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//C9 = Identify Object
	ConnUser[User].EventCount++;

	memcpy(PackPointer, &GUID, 4);	PackPointer += 4;

	//Figure out Flags...	
	switch (cwoItem->Type)
	{
	case OBJECT_CHARACTER:
		tpd = 0x00000080;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags - Char/Monster
		break;
	case OBJECT_PORTAL:
		tpd = 0x00200000;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Flags - Portal
		break;
	}

	tpd = 0x0001;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Successful Assess

	//Build Packet
	if (cwoItem->Type == OBJECT_CHARACTER)
	{
		tpd = 0x0000000C;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//characterFlags
		memcpy(PackPointer, &ConnUser[User].Char->Level, 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentSecondaryStat[0], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->BaseSecondaryStat[0], 4);	PackPointer += 4;

		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[0], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[1], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[3], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[2], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[4], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[5], 4);	PackPointer += 4;

		memcpy(PackPointer, &ConnUser[User].Char->CurrentSecondaryStat[1], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->BaseSecondaryStat[1], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentSecondaryStat[2], 4);	PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->BaseSecondaryStat[2], 4);	PackPointer += 4;

		tpd = 0;			memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Species
	}
	else if (cwoItem->Type == OBJECT_PORTAL)
	{
		cWorldObject_Portal *tpp = (cWorldObject_Portal *) cwoItem;
		DWORD Lower, Upper;
		Lower = (tpp->Flags & 0x00FF0000) >> 16;
		if (!Lower) Lower = 0xFFFFFFFF;
		Upper = (tpp->Flags & 0xFF000000) >> 24;
		if (!Upper) Upper = 0xFFFFFFFF;

		memcpy(PackPointer, &Lower, 4);	PackPointer += 4;		//Lower Limit
		memcpy(PackPointer, &Upper, 4);	PackPointer += 4;		//Upper Limit

		int packlen;
		GenString(tpp->Name, &packlen, PackPointer);  PackPointer += packlen;
	}
	
	Send200(OutPack, (DWORD) PackPointer - (DWORD) OutPack, 4, User);

	ActionComplete(User);
}

void cACServer::ServerMessage(int User, char *Text, DWORD Color)
{
	try {
		BYTE TpMes[1024]; BYTE * PackPtr = &TpMes[0];
		DWORD tpf;
		int outlen;
		tpf = 0x0000F62C;		memcpy(PackPtr,&tpf,4);	PackPtr += 4;
		GenString(Text, &outlen, PackPtr); PackPtr += outlen;
		tpf = Color;		memcpy(PackPtr,&tpf,4);	PackPtr += 4;
		if ((User >= 0) && (User < 128))
		{
			Send200(&TpMes[0], (int) (PackPtr - &TpMes[0]), 4, User);
		} else if (User == GLOBAL_MESSAGE) {
			//Global Message...
			for (int i=0;i<MaxUsers;i++)
			{
				if ((ConnUser[i].Connected) && (ConnUser[i].State >= 5))
					Send200(&TpMes[0], (int) (PackPtr - &TpMes[0]), 4, i);
			}
		} else if ((User > -128) && (User < 0)) {
			//Global Message except User
			for (int i=0;i<MaxUsers;i++)
			{
				if ((ConnUser[i].Connected) && (ConnUser[i].State >= 5) && (i != (User*-1)-1))
					Send200(&TpMes[0], (int) (PackPtr - &TpMes[0]), 4, i);
			}
		}
	} catch (char * ) {
		//Meh...
		
	}
}

void cACServer::UpdateSkill(int User, int Skill)
{
	//Skill update - send 23E
	BYTE Pack23E[40]; WORD tpw = Skill; DWORD tpdword; BYTE tpbyte;
	tpdword = 0x023E;	memcpy(&Pack23E[0], &tpdword, 4);
						memcpy(&Pack23E[4], &tpw, 2);
	tpbyte = 0;			memcpy(&Pack23E[6], &tpbyte, 1);
	tpbyte = ConnUser[User].Count23E;			memcpy(&Pack23E[7], &tpbyte, 1);
						memcpy(&Pack23E[8], &ConnUser[User].Char->SkillInc[Skill], 4);
						memcpy(&Pack23E[12], &ConnUser[User].Char->SkillTrain[Skill], 4);
						memcpy(&Pack23E[16], &ConnUser[User].Char->SkillXP[Skill], 4);
						memcpy(&Pack23E[20], &ConnUser[User].Char->SkillFreeXP[Skill], 4);
	tpdword = 0;		memcpy(&Pack23E[24], &tpdword, 4);
	DWORD TPT23EShit[3] = {
		0xE34B9579,
		0x4175FD72,
		0x003AF0F8
	};
	memcpy(&Pack23E[28], TPT23EShit, 12);
	Send200(Pack23E, 40, 4, User);
	ConnUser[User].Count23E++;

	ConnUser[User].Char->SkillBase[Skill] = 0;
	if (SkillInfo[Skill].AttribA != STAT_NULL) ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->CurrentStat[SkillInfo[Skill].AttribA];
	if (SkillInfo[Skill].AttribB != STAT_NULL) ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->CurrentStat[SkillInfo[Skill].AttribB];
	ConnUser[User].Char->SkillBase[Skill] /= SkillInfo[Skill].Divider;
	ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->SkillInc[Skill];
}

void cACServer::UpdateStat(int User, int Stat)
{
	//Stat update - send 241
	BYTE Pack241[20]; WORD tpw = Stat+1; DWORD tpdword; BYTE tpbyte;
	if (tpw == 3) tpw = 4;
	else if (tpw == 4) tpw = 3;
	tpdword = 0x0241;	memcpy(&Pack241[0], &tpdword, 4);
						memcpy(&Pack241[4], &tpw, 2);
	tpbyte = 0;			memcpy(&Pack241[6], &tpbyte, 1);
	tpbyte = ConnUser[User].Count23E;			memcpy(&Pack241[7], &tpbyte, 1);
						memcpy(&Pack241[8], &ConnUser[User].Char->CurrentStat[Stat], 4);
						memcpy(&Pack241[12], &ConnUser[User].Char->InitialStat[Stat], 4);
						memcpy(&Pack241[16], &ConnUser[User].Char->XPIntoStat[Stat], 4);

	Send200(Pack241, 20, 4, User);
	ConnUser[User].Count23E++;

	for (int Skill=1;Skill<40;Skill++)
	{
		ConnUser[User].Char->SkillBase[Skill] = 0;
		if (SkillInfo[Skill].AttribA != STAT_NULL) ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->CurrentStat[SkillInfo[Skill].AttribA];
		if (SkillInfo[Skill].AttribB != STAT_NULL) ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->CurrentStat[SkillInfo[Skill].AttribB];
		ConnUser[User].Char->SkillBase[Skill] /= SkillInfo[Skill].Divider;
		ConnUser[User].Char->SkillBase[Skill] += ConnUser[User].Char->SkillInc[Skill];
	}
}

void cACServer::UpdateSecondaryStat(int User, int Stat)
{
	if (Stat == 0) ConnUser[User].Char->BaseSecondaryStat[Stat] = ConnUser[User].Char->SecondaryStatInc[Stat] + (ConnUser[User].Char->CurrentStat[1] >> 1);
	if (Stat == 1) ConnUser[User].Char->BaseSecondaryStat[Stat] = ConnUser[User].Char->SecondaryStatInc[Stat] + ConnUser[User].Char->CurrentStat[1];
	if (Stat == 2) ConnUser[User].Char->BaseSecondaryStat[Stat] = ConnUser[User].Char->SecondaryStatInc[Stat] + ConnUser[User].Char->CurrentStat[5];

	//Secondary Stat update - send 243
	BYTE Pack243[24]; WORD tpw = 1+(Stat<<1); DWORD tpdword; BYTE tpbyte;
	tpdword = 0x0243;	memcpy(&Pack243[0], &tpdword, 4);
						memcpy(&Pack243[4], &tpw, 2);
	tpbyte = 0;			memcpy(&Pack243[6], &tpbyte, 1);
	tpbyte = ConnUser[User].Count23E;			memcpy(&Pack243[7], &tpbyte, 1);
						memcpy(&Pack243[8], &ConnUser[User].Char->SecondaryStatInc[Stat], 4);
	tpdword = 0;		memcpy(&Pack243[12], &tpdword, 4);
						memcpy(&Pack243[16], &ConnUser[User].Char->XPIntoSecondaryStat[Stat], 4);
						memcpy(&Pack243[20], &ConnUser[User].Char->BaseSecondaryStat[Stat], 4);
	Send200(Pack243, 24, 4, User);
	ConnUser[User].Count23E++;
}

int cACServer::Generate_F7B0_13(BYTE * OutPack, int User)
{
	BYTE *PackPointer = OutPack;
	DWORD tpd;	WORD tpw;
	
	//F7b0 header
	tpd = 0xF7B0;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
						memcpy(PackPointer, &ConnUser[User].Char->GUID, 4);	PackPointer += 4;
						memcpy(PackPointer, &ConnUser[User].EventCount, 4);	PackPointer += 4;
	tpd = 0x0013;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//13 = Login Char
	ConnUser[User].EventCount++;
	
	//On the the actual packet... - loginmask1 - 7B in the stuff...
	tpd = 0x001B;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//loginMask1
	tpd = 0x000A;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown1 - A
	
	//count1
	tpw = 0x000C;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//count for next section
	tpw = 0x0040;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//unknown for this - 40

	tpd = 0x0081;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	tpd = 0x02F7;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;

	tpd = 0x0084;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	tpd = 0x0004;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;

	//Burden, calc later..
	tpd = 0x0005;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//5 = burden
	tpd =      1;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//1 = ?

	tpd = 0x008B;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	tpd = 0x0037;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;

	//Pyreals, calc later
	tpd = 0x0014;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//14 = pyreals
	tpd =      0;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//0
	//Total Xp
	tpd = 0x0015;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//15 = total xp
						memcpy(PackPointer, &ConnUser[User].Char->TotalXP, 4);	PackPointer += 4;
	//Unassigned XP
	tpd = 0x0016;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//16 = unassigned xp
						memcpy(PackPointer, &ConnUser[User].Char->UnassignedXP, 4);	PackPointer += 4;
	//Unassigned Skill Points
	tpd = 0x0018;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//18 = unassigned skill points
						memcpy(PackPointer, &ConnUser[User].Char->SkillCredits, 4);	PackPointer += 4;
	//Level, calc later....
	tpd = 0x0019;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//19 = Level
						memcpy(PackPointer, &ConnUser[User].Char->Level, 4);	PackPointer += 4;
	//Rank, calc later
	tpd = 0x001e;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//1e = Rank
	tpd = 0x0000;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;

	tpd = 0x0028;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	tpd = 0x0001;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;

	tpd = 0x002F;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	tpd = 0x0000;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
//end of that vector

	//count2	
	tpw = 0x0005;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//count2
	tpw = 0x0020;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//unknown - 20
	//count2 vector here... i.e. nothing yet

	//Count2 Stuff...
	DWORD CHAR_DATA_VECTOR2[] = {
		0x00000004, 0x00000000, 0x0000002C, 0x00000000, 
		0x0000002D, 0x00000000, 0x0000002E, 0x00000000, 
		0x0000002F, 0x00000000, 
	};
	memcpy(PackPointer, CHAR_DATA_VECTOR2, sizeof(CHAR_DATA_VECTOR2));	PackPointer += sizeof(CHAR_DATA_VECTOR2);

	//strings
	tpw = 0x0004;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//stringcount
	tpw = 0x0010;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//strings unknown - 10

	//Char Name
	tpd = 0x0001;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
						GenString(ConnUser[User].Char->Name, (int *) &tpd, PackPointer);	PackPointer += tpd;
	//Gender
	tpd = 0x0003;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	char Gender[7];
	if (ConnUser[User].Char->Gender == 0)
		sprintf(Gender, "Female");
//	if (ConnUser[User].Char->Gender == 1)
	else
		sprintf(Gender, "Male");
	GenString(Gender, (int *) &tpd, PackPointer);	PackPointer += tpd;

	//Race
	char Race[12];
	tpd = 0x0004;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	if (ConnUser[User].Char->Race == 0)
		sprintf(Race, "Aluvian");
	if (ConnUser[User].Char->Race == 1)
		sprintf(Race, "Gharun'Dim");
//	if (ConnUser[User].Char->Race == 2)
	else
		sprintf(Race, "Sho");
	GenString(Race, (int *) &tpd, PackPointer);	PackPointer += tpd;

	//Class	- "Server Tester" for now... ;-)
	tpd = 0x0005;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
	GenString(ConnUser[User].Char->Title, (int *) &tpd, PackPointer);	PackPointer += tpd;

	//count4
	tpw = 0x0001;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//count4
	tpw = 0x0020;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//count4unknown - 20
	//count4 vector here... i.e. nothing yet

	//sec4 stuff
	tpd = 0x0004;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//
	tpd = 0x30000000;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//

	//Loginmask1 info goes here

	

	//End loginmask1 shit
	//should be 103, 100 = spellbook, 2 = skills
	tpd = 0x0103;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Loginmask2
	tpd = 0x0001;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown14 - always 1
	tpd = 0x01ff;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//attributemask - always 0x1ff
	
	//Primary Stats						
	for (int i=0;i<6;i++)
	{
		if (i == 3)
		{
			memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[2], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->InitialStat[2], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->XPIntoStat[2], 4);	PackPointer += 4;
		} else if (i == 2) {
			memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[3], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->InitialStat[3], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->XPIntoStat[3], 4);	PackPointer += 4;
		} else {
			memcpy(PackPointer, &ConnUser[User].Char->CurrentStat[i], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->InitialStat[i], 4);	PackPointer += 4;
			memcpy(PackPointer, &ConnUser[User].Char->XPIntoStat[i], 4);	PackPointer += 4;
		}
	}

	//Secondary Stats
	for (i=0;i<3;i++)
	{
		memcpy(PackPointer, &ConnUser[User].Char->SecondaryStatInc[i], 4);		PackPointer += 4;
		tpd = 0;	memcpy(PackPointer, &tpd, 4);		PackPointer += 4;	//?... always 0 afawct
		memcpy(PackPointer, &ConnUser[User].Char->XPIntoSecondaryStat[i], 4);		PackPointer += 4;
		memcpy(PackPointer, &ConnUser[User].Char->CurrentSecondaryStat[i], 4);		PackPointer += 4;
	}

	//Skillcount
	tpw = 0x0023;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//count
	tpw = 0x0020;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//unknown - 20
	for (i=1;i<40;i++)
	{
		if (ConnUser[User].Char->SkillTrain[i] > 0)
		{
			memcpy(PackPointer, &i, 4);	PackPointer += 4;		//Skill number
			memcpy(PackPointer, &ConnUser[User].Char->SkillInc[i], 4);	PackPointer += 4;		//Skill number
			memcpy(PackPointer, &ConnUser[User].Char->SkillTrain[i], 4);	PackPointer += 4;		//Trained Status
			memcpy(PackPointer, &ConnUser[User].Char->SkillXP[i], 4);	PackPointer += 4;		//Total XP into skill
			memcpy(PackPointer, &ConnUser[User].Char->SkillFreeXP[i], 4);	PackPointer += 4;	//Free XP at gen
			tpd = 0x0000;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;	//unknown2
			
			//double thingy
			tpd = 0xE5C9D958;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
			tpd = 0x4171BA45;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;
		}
	}

	//Loginmask2 time
	//100 - spellbook
	WORD numspells = 0;
	for (i=0;i<96;i++)
	{
		for (DWORD tpdd=ConnUser[User].Char->SpellsLearned[i];tpdd > 0;tpdd >>= 1)
		{
			if (tpdd & 1)
				numspells++;
		}
	}
						memcpy(PackPointer, &numspells, 2);	PackPointer += 2;		//count
	tpw = 0x0040;		memcpy(PackPointer, &tpw, 2);	PackPointer += 2;		//spellcountunknown

	int tpinc;	float tpcharge = 1.0f;
	for (i=0;i<96;i++)
	{
		tpinc = 0;
		for (DWORD TpSpell = ConnUser[User].Char->SpellsLearned[i]; TpSpell > 0; TpSpell >>= 1)
		{
			if (TpSpell & 1)
			{
				tpd = (i << 5) | tpinc;		memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//spell ID
											memcpy(PackPointer, &tpcharge, 4);	PackPointer += 4;		//charge
				tpd = 0;					memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown2
				tpd = 0;					memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown3
			}
			tpinc++;
		}
	}

	//200 - enchantments... whoooa there cowboy

	DWORD Numshorts = 0;
	for (i=0;i<10;i++)
		if (ConnUser[User].Char->ShortCut[i])
			Numshorts++;

	if (Numshorts)
		tpd = 0x0005;
	else
		tpd = 0x0004;
						memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//loginmask3

	//4, character options
//	tpd = 0x01ECE563;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//loginmask3
	tpd = 0x01E4E56A;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//loginmask3

	if (Numshorts)
	{
		//1, shortcut bar
		memcpy(PackPointer, &Numshorts, 4);	PackPointer += 4;
		for (i=0;i<10;i++)
			if (ConnUser[User].Char->ShortCut[i])
			{
				memcpy(PackPointer, &i, 4);	PackPointer += 4;									//Position
				memcpy(PackPointer, &ConnUser[User].Char->ShortCut[i], 4);	PackPointer += 4;	//Object
				tpd = 0;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;					//Unknown - 0
			}
	}

	//Spellbars
	for (i=0;i<5;i++)
	{
		DWORD numspells = 0;
		for (numspells=0;ConnUser[User].Char->SpellBar[i][numspells] > 0;numspells++) { }

		memcpy(PackPointer, &numspells, 4);	PackPointer += 4;
		for (int j=0;j<numspells;j++)
		{
			memcpy(PackPointer, &ConnUser[User].Char->SpellBar[i][j], 4);	PackPointer += 4;
		}
	}

	//comp buyer...
	
	//Inventory
	DWORD numinv = 0;
	for (i=0;i<7;i++)
		if (ConnUser[User].Char->SidePacks[i])
			numinv++;
	for (i=0;i<102;i++)
		if (ConnUser[User].Char->MainPack[i])
			numinv++;

	memcpy(PackPointer, &numinv, 4);	PackPointer += 4;
	for (i=0;i<7;i++)
		if (ConnUser[User].Char->SidePacks[i])
		{
			memcpy(PackPointer, &ConnUser[User].Char->SidePacks[i], 4);	PackPointer += 4;
			tpd = 1;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//IsContainer = 1
		}
	for (i=0;i<102;i++)
		if (ConnUser[User].Char->MainPack[i])
		{
			memcpy(PackPointer, &ConnUser[User].Char->MainPack[i], 4);	PackPointer += 4;
			tpd = 0;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//IsContainer = 0
		}

	//Equipped Shizzit
	DWORD numequip = 0;
	for (i=0;i<25;i++)
		if (ConnUser[User].Char->Equipped[i])
			numequip++;

	memcpy(PackPointer, &numequip, 4);	PackPointer += 4;
	for (i=0;i<25;i++)
		if (ConnUser[User].Char->Equipped[i])
		{
			memcpy(PackPointer, &ConnUser[User].Char->Equipped[i], 4);	PackPointer += 4;
			tpd = (1 << i);	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//Coverage
			tpd = 0;	memcpy(PackPointer, &tpd, 4);	PackPointer += 4;		//unknown3
		}

	return((int) (PackPointer - OutPack));
}

