#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "swap.h"
#include "wad.h"

typedef struct
{
	char id[4];
	uint32_t numlumps;
	uint32_t lumplistpos;
} header_t;

int WAD_LoadFile(const char* fn, wad_t* wad)
{
	header_t header;
	
	wad->fp = fopen(fn, "rb");
	if (!wad->fp) return W_FOPENERROR;
	
	//Read WAD header
	if (fread(&header, 1, sizeof(header), wad->fp) < sizeof(header))
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
	
	//Get number of lumps available and lumplist offset
	wad->numlumps = swapU32(header.numlumps);
	wad->lumplistpos = swapU32(header.lumplistpos);
	
	return 0;
}

typedef struct
{
	uint32_t pos;
	uint32_t size;
	char name[8];
} lumpinfo_t;

int WAD_LoadLumpData(const char* ln, wad_t* wad, lump_t* lump)
{
	uint32_t numlumps = wad->numlumps;
	lumpinfo_t lumpinfo;
	
	//Go to the lump list
	if (fseek(wad->fp, wad->lumplistpos, SEEK_SET))
		return W_FTRUNCATED;
	
	//Look for the desired lump in the list
	uint32_t i;
	for(i=0; i < numlumps && !feof(wad->fp); ++i)
	{
		if (fread(&lumpinfo, 1, sizeof(lumpinfo), wad->fp) < sizeof(lumpinfo))
			return W_FTRUNCATED;
		
		if (memcmp(lumpinfo.name, ln, strlen(ln)&15))
			continue;
		
		//If we found the desired lump...
		
		//Go to where is the lump's data
		if (fseek(wad->fp, swapU32(lumpinfo.pos), SEEK_SET))
			return W_FTRUNCATED;
		
		//malloc raw data to be filled
		lump->size = swapU32(lumpinfo.size);
		lump->data = calloc(lump->size, 1);
		if (!lump->data) return W_LMALLOCERR;
		
		//Read raw data from the file
		if (fread(lump->data, 1, lump->size, wad->fp) < lump->size)
		{
			free(lump->data);
			return W_FTRUNCATED;
		}
		
		return 0;
	}
	return W_LUMPNOTFOUND;
}

int WAD_PrintLumpList(wad_t* wad)
{
	lumpinfo_t lumpinfo;
	
	//Go to the lump list
	if (fseek(wad->fp, wad->lumplistpos, SEEK_SET))
		return W_FTRUNCATED;
	
	printf("Lump List: \n\n");
	
	//Print a list containing all lumps available to screen
	while(!feof(wad->fp))
	{
		if (fread(&lumpinfo, 1, sizeof(lumpinfo), wad->fp) < sizeof(lumpinfo))
			return W_FTRUNCATED;
		
		printf("fp:0x%X, size:%u, name:%.8s\n", swapU32(lumpinfo.pos), swapU32(lumpinfo.size), lumpinfo.name);
	}
}

void WAD_FreeLumpData(lump_t* lump)
{
	free(lump->data);
	memset(lump, 0, sizeof(lump));
}

void WAD_Close(wad_t* wad)
{
	fclose(wad->fp);
}