
DWORD GetMagicNumber(BYTE * buf, UINT cb, BOOL bIncludeSize)
{
	DWORD dwCS = 0;
	UINT i;
	if ( bIncludeSize )
	{
		dwCS += cb<<16;
	}
	// sum up the DWORDs:
	for (  i = 0; i<(cb>>2); i++ )
	{
		dwCS += ((DWORD *)buf)[i];
	}
	// now any remaining bytes are summed in reverse endian
	int shift=3;
	for ( i=(i<<2); i<cb; i++ )
	{
		dwCS += buf[i]<<(shift*8);
		shift--;
	}
	return dwCS;
}

