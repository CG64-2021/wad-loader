#ifndef __WAD_H__
#define __WAD_H__

//You can find more info about WAD file here:
//https://doomwiki.org/wiki/WAD

//Error codes, used by WAD_LoadFile()
#define W_FOPENERROR 1 //if fopen() fails
#define W_NONVALID 2 //if a file is not a valid wad file
#define W_LMALLOCERROR 3 //if the lump list couldn't be mallocated
#define W_LFETCHERROR 4 //if the lump list wasn't fetched completely

typedef struct
{
	uint32_t filepos;
	uint32_t size;
	char name[8];
} lump_t;

typedef struct
{
	FILE* fp;
	uint32_t numlumps;
	lump_t* lumps;
} wad_t;

//Load a WAD file and its lump list
//Don't forget to WAD_Close() if you won't use it anymore
//Args: filename, wad_t addr
//Return: 0 if sucess, any error code if fail 
int WAD_LoadFile(const char* fn, wad_t* wad);

//Load lump raw data from the wad
//Don't forget to free() if you won't use it anymore
//Args: lumpname, wad_t addr
//Return: ptr to raw data mallocated, NULL if fail 
uint8_t* WAD_LoadLumpData(const char* ln, wad_t* wad);

//Free mallocated elements within wad_t and close WAD file
//Args: wad_t addr
void WAD_Close(wad_t* wad);

#endif