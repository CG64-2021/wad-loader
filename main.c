#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "wad.h"


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("wad_loader.exe <filename>\n");
		return 0;
	}
	
	int ret;
	wad_t mywad;
	if (ret = WAD_LoadFile(argv[1], &mywad))
	{
		printf("Failed to load WAD file! Errno:%d\n", ret);
		return 0;
	}
	
	printf("List of lumps available:\n\n");
	uint32_t i;
	for(i=0; i < mywad.numlumps; ++i)
	{
		printf("fp:0x%X, size:%u, name:%.8s\n", mywad.lumps[i].filepos, mywad.lumps[i].size, mywad.lumps[i].name);
	}
	
	char lumpname[9];
	printf("\nWhich lump you want to load?\nName: ");
	scanf("%8s", lumpname);
	
	lumpdata_t lumpdata;
	if (ret = WAD_LoadLumpData(lumpname, &mywad, &lumpdata))
	{
		printf("Failed to load lump data! errno:%d\n", ret);
		WAD_Close(&mywad);
		return 0;
	}
	
	uint8_t* bytes = lumpdata.data;
	printf("Lump data loaded successful!\n");
	printf("size: %u\n", lumpdata.size);
	
	WAD_FreeLumpData(&lumpdata);
	WAD_Close(&mywad);
	
	return 0;
}