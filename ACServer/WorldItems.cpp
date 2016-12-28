#include "stdafx.h"
#include "WorldItems.h"

void GenString(char *str2c, int *outlen, BYTE *outbuf)
{
	BYTE *PackPointer = outbuf;

	WORD NameLen = strlen(str2c) + 1;
	memcpy(PackPointer,&NameLen,sizeof(NameLen)); PackPointer += sizeof(NameLen);	//NameLen
	NameLen--; memcpy(PackPointer,str2c,NameLen); PackPointer += NameLen;	//Name
	*PackPointer = 0; PackPointer++;												//0
	int Tpl = ((PackPointer-outbuf)%4); if (Tpl) Tpl=4-Tpl;			//Padding
	for (int ii=0;ii<Tpl;ii++) {	*PackPointer = 0; PackPointer++; }		//"
	*outlen = (int) (PackPointer - outbuf);
}
