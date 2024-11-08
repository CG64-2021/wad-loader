#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "swap.h"
#include "wad.h"

#define HEADER_SIZE 12
#define LUMPINFO_SIZE 16

static int WAD_Checksum(wad_t* wad)
{
	if (memcmp(wad->id, "IWAD", 4) && memcmp(wad->id, "PWAD", 4))
	{
		fclose(wad->fp);
		return W_NONVALID;
	}
	
	if (fseek(wad->fp, 0, SEEK_END))
		return W_FTRUNCATED;
	
	long int pos = ftell(wad->fp);
	uint32_t checksum = pos - wad->numlumps*LUMPINFO_SIZE;
	if (checksum != wad->lumplistpos)
		return W_NONVALID;
	
	return 0;
}

int WAD_LoadFile(const char* fn, wad_t* wad)
{
	char buffer[HEADER_SIZE];
	
	wad->fp = fopen(fn, "rb");
	if (!wad->fp) return W_FOPENERROR;
	
	//Read WAD header
	if (fread(buffer, 1, HEADER_SIZE, wad->fp) < HEADER_SIZE)
	{
		fclose(wad->fp);
		return W_FTRUNCATED;
	}
	memcpy(wad->id, buffer, 4);
	memcpy(&wad->numlumps, &buffer[4], 4);
	memcpy(&wad->lumplistpos, &buffer[8], 4);
	wad->numlumps = swapU32(wad->numlumps);
	wad->lumplistpos = swapU32(wad->lumplistpos);
	
	//Check if it's a valid WAD file
	return WAD_Checksum(wad);
}

int WAD_LoadLumpData(const char* ln, wad_t* wad, lump_t* lump)
{
	char buffer[LUMPINFO_SIZE];
	uint32_t numlumps = wad->numlumps;
	uint32_t lumppos;
	uint32_t lumpsize;
	
	//Go to the lump list
	if (fseek(wad->fp, wad->lumplistpos, SEEK_SET))
		return W_FTRUNCATED;
	
	//Look for the desired lump in the list
	while(!feof(wad->fp))
	{
		memset(buffer, 0, LUMPINFO_SIZE);
		if (fread(buffer, 1, LUMPINFO_SIZE, wad->fp) < LUMPINFO_SIZE)
			return W_FTRUNCATED;
		
		//If we found the desired lump...
		if (memcmp(&buffer[8], ln, strlen(ln)&(LUMPINFO_SIZE-1)))
			continue;
		
		//Go to where is the lump's data
		memcpy(&lumppos, buffer, 4);
		if (fseek(wad->fp, swapU32(lumppos), SEEK_SET))
			return W_FTRUNCATED;
		
		//malloc raw data to be filled
		memcpy(&lumpsize, &buffer[4], 4);
		lump->size = swapU32(lumpsize);
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
	char buffer[LUMPINFO_SIZE];
	char lumpname[9];
	uint32_t lumppos;
	uint32_t lumpsize;
	
	//Go to the lump list
	if (fseek(wad->fp, wad->lumplistpos, SEEK_SET))
		return W_FTRUNCATED;
	
	printf("Lump List: \n\n");
	
	//Print a list containing all lumps available to screen
	while(!feof(wad->fp))
	{
		memset(buffer, 0, LUMPINFO_SIZE);
		memset(lumpname, 0, 9);
		if (fread(buffer, 1, LUMPINFO_SIZE, wad->fp) < LUMPINFO_SIZE)
			return W_FTRUNCATED;
		
		memcpy(&lumppos, buffer, 4);
		memcpy(&lumpsize, &buffer[4], 4);
		memcpy(&lumpname, &buffer[8], 8);
		
		printf("fp:0x%X, size:%u, name:%s\n", swapU32(lumppos), swapU32(lumpsize), lumpname);
	}
	return 0;
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