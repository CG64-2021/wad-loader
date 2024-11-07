#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "wad.h"

typedef struct
{
	char id[4];
	uint32_t numlumps;
	uint32_t lumplistofs;
} header_t;


int WAD_LoadFile(const char* fn, wad_t* wad)
{
	header_t header;
	
	wad->fp = fopen(fn, "rb");
	if (!wad->fp) return W_FOPENERROR;
	
	//Read WAD header
	if (fread(&header, 1, sizeof(header_t), wad->fp) < sizeof(header))
	{
		fclose(wad->fp);
		return W_FTRUNCATED;
	}
	
	//Check if its a valid WAD file
	if (memcmp(header.id, "IWAD", 4) && memcmp(header.id, "PWAD", 4))
	{
		fclose(wad->fp);
		return W_NONVALID;
	}
	
	//Get number of lumps available
	wad->numlumps = header.numlumps;
	
	//Go to the lump list
	if (fseek(wad->fp, header.lumplistofs, SEEK_SET))
	{
		fclose(wad->fp);
		return W_FTRUNCATED;
	}
	
	//Get full list of available lumps
	wad->lumps = (lump_t*)malloc(wad->numlumps*sizeof(lump_t));
	if (!wad->lumps)
	{
		fclose(wad->fp);
		return W_LMALLOCERROR;
	}
	memset(wad->lumps, 0, sizeof(wad->lumps));
	if (fread(wad->lumps, sizeof(wad->lumps[0]), wad->numlumps, wad->fp) < sizeof(wad->lumps))
	{
		free(wad->lumps);
		fclose(wad->fp);
		return W_LFETCHERROR;
	}
	
	return 0;
}

int WAD_LoadLumpData(const char* ln, wad_t* wad, uint8_t* raw_data)
{
	uint32_t numlumps = wad->numlumps;
	lump_t* lumps = wad->lumps;
	
	//Look for the desired lump in the list
	uint32_t i;
	for(i=0; i < numlumps; ++i)
	{
		//A lump name is 8 bytes long
		//If the desired lump was found, let's get its raw data
		if (!memcmp(lumps[i].name, ln, sizeof(ln)&15))
		{
			if (fseek(wad->fp, lumps[i].filepos, SEEK_SET))
			{
				return W_FTRUNCATED;
			}
			raw_data = malloc(lumps[i].size);
			if (!raw_data) return W_DATAMALLOCERROR;
			memset(raw_data, 0, sizeof(raw_data));
			
			if (fread(raw_data, 1, lumps[i].size, wad->fp) < lumps[i].size)
			{
				free(raw_data);
				return W_DATAFETCHERROR;
			}
			return 0;
		}
	}
	return W_LUMPNOTFOUND;
}

void WAD_Close(wad_t* wad)
{
	fclose(wad->fp);
	free(wad->lumps);
}
