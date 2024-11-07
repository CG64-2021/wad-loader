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
	
	//Printf list of lumps available
	WAD_PrintLumpList(&mywad);
	
	char lumpname[9];
	printf("\nWhich lump you want to load?\nName: ");
	scanf("%8s", lumpname);
	
	lump_t lump;
	if (ret = WAD_LoadLumpData(lumpname, &mywad, &lump))
	{
		printf("Failed to load lump data! errno:%d\n", ret);
		WAD_Close(&mywad);
		return 0;
	}
	
	uint8_t* bytes = lump.data;
	printf("Lump data loaded successful!\n");
	printf("size: %u\n", lump.size);
	
	WAD_FreeLumpData(&lump);
	WAD_Close(&mywad);
	
	return 0;
}