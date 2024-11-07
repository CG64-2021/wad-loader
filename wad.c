#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "wad.h"


int WAD_LoadFile(const char* fn, wad_t* wad)
{
	char id[4];
	uint32_t lumplistofs;
	
	//Open a file
	wad->fp = fopen(fn, "rb");
	if (!wad->fp) return W_FOPENERROR;
	
	//Check if it is a valid wad file
	fread(id, 1, 4, wad->fp);
	if (memcmp(id, "IWAD", 4) && memcmp(id, "PWAD", 4))
	{
		fclose(wad->fp);
		return W_NONVALID;
	}
	
	//How many lumps we have in the wad?
	fread(&wad->numlumps, 4, 1, wad->fp);
	
	//Let's go to the lumps list
	fread(&lumplistofs, 4, 1, wad->fp);
	fseek(wad->fp, lumplistofs, SEEK_SET);
	
	//Now let's get the full list of available lumps from the wad
	wad->lumps = (lump_t*)malloc(wad->numlumps*sizeof(lump_t));
	if (!wad->lumps)
	{
		fclose(wad->fp);
		return W_LMALLOCERROR;
	}
	memset(wad->lumps, 0, sizeof(wad->lumps));
	size_t numbytes_read = fread(wad->lumps, sizeof(wad->lumps[0]), wad->numlumps, wad->fp);
	if (numbytes_read < sizeof(wad->lumps))
	{
		printf("W_LoadFile: Failed to load all data! Total:%u Read:%u\n", sizeof(wad->lumps), numbytes_read);
		free(wad->lumps);
		fclose(wad->fp);
		return W_LFETCHERROR;
	}
	
	return 0;
}

uint8_t* WAD_LoadLumpData(const char* ln, wad_t* wad)
{
	uint8_t* data;
	
	uint32_t numlumps = wad->numlumps;
	lump_t* lumps = wad->lumps;
	
	//Look for the desired lump in the list
	uint32_t i;
	for(i=0; i < numlumps; ++i)
	{
		//A lump name has 8 characters total
		//If the desired lump was found, let's get its raw data
		if (!memcmp(lumps[i].name, ln, 8))
		{
			fseek(wad->fp, lumps[i].filepos, SEEK_SET);
			data = malloc(lumps[i].size);
			if (!data) return NULL;
			memset(data, 0, sizeof(data));
			size_t numbytes_read = fread(data, 1, lumps[i].size, wad->fp);
			if (numbytes_read < (lumps[i].size))
			{
				printf("WAD_LoadLumpData: Failed to load all data! Total:%d Read:%d\n", lumps[i].size, numbytes_read);
				free(data);
				return NULL;
			}
			return data;
		}
	}
	return NULL;
}

void WAD_Close(wad_t* wad)
{
	fclose(wad->fp);
	free(wad->lumps);
}