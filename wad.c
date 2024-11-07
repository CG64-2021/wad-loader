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
	uint32_t lumplistofs;
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
	
	//Get number of lumps available
	wad->numlumps = swapU32(header.numlumps);
	
	//Go to the lump list
	if (fseek(wad->fp, swapU32(header.lumplistofs), SEEK_SET))
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
	memset(wad->lumps, 0, wad->numlumps*sizeof(lump_t));
	if (fread(wad->lumps, 1, wad->numlumps*sizeof(wad->lumps[0]), wad->fp) < wad->numlumps*sizeof(wad->lumps[0]))
	{ 
		free(wad->lumps);
		fclose(wad->fp);
		return W_LFETCHERROR;
	}
	
	return 0;
}

int WAD_LoadLumpData(const char* ln, wad_t* wad, lumpdata_t* lumpdata)
{
	uint32_t numlumps = wad->numlumps;
	lump_t* lumps = wad->lumps;
	
	//Look for the desired lump in the list
	uint32_t i;
	for(i=0; i < numlumps; ++i)
	{
		//A lump name is 8 bytes long
		if (!memcmp(lumps[i].name, ln, strlen(ln)&15))
		{
			if (fseek(wad->fp, swapU32(lumps[i].filepos), SEEK_SET))
			{
				return W_FTRUNCATED;
			}
			
			//Get lump size
			lumpdata->size = swapU32(lumps[i].size);
			
			//Malloc raw_data
			lumpdata->data = malloc(lumpdata->size);
			if (!lumpdata->data) return W_DATAMALLOCERROR;
			memset(lumpdata->data, 0, lumpdata->size);
			
			//Read raw data from the file
			if (fread(lumpdata->data, 1, lumpdata->size, wad->fp) < lumpdata->size)
			{
				WAD_FreeLumpData(lumpdata);
				return W_DATAFETCHERROR;
			}
			return 0;
		}
	}
	return W_LUMPNOTFOUND;
}

void WAD_FreeLumpData(lumpdata_t* lumpdata)
{
	free(lumpdata->data);
	memset(lumpdata, 0, sizeof(lumpdata));
}

void WAD_Close(wad_t* wad)
{
	fclose(wad->fp);
	free(wad->lumps);
}
