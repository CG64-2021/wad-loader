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
	
	wad_t mywad;
	int ret = WAD_LoadFile(argv[1], &mywad);
	
	if (ret == 4) return 0;
	
	printf("List of lumps available:\n\n");
	uint32_t i;
	for(i=0; i < mywad.numlumps; ++i)
	{
		printf("fp:0x%X, size:%u, name:%.8s\n", mywad.lumps[i].filepos, mywad.lumps[i].size, mywad.lumps[i].name);
	}
	
	WAD_Close(&mywad);
	
	return 0;
}