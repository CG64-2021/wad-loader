#ifndef __WAD_H__
#define __WAD_H__

//You can find more info about WAD file here:
//https://doomwiki.org/wiki/WAD

//Error codes, used by WAD_LoadFile()
#define W_FOPENERROR 1 //if fopen() fails
#define W_FTRUNCATED 2 //if the file is truncated
#define W_NONVALID 3 //if a file is not a valid wad file
#define W_LMALLOCERROR 4 //if the lump list couldn't be mallocated
#define W_LFETCHERROR 5 //if the lump list wasn't fetched completely

//Error codes, used by WAD_LoadLumpData()
#define W_DATAMALLOCERROR 6 //if raw_data wasn't mallocated
#define W_DATAFETCHERROR 7 //if lump data couldn't be fetched from the file
#define W_LUMPNOTFOUND 8 //if desired lump was not found

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
//Return: 0 if success, errno if fail 
int WAD_LoadFile(const char* fn, wad_t* wad);

//Get lump raw data from the wad
//Don't forget to free() if you won't use raw_data anymore
//Args: lumpname, wad_t addr, raw_data
//Return: 0 if success, errno if fail
int WAD_LoadLumpData(const char* ln, wad_t* wad, uint8_t* raw_data);

//Free mallocated elements within wad_t and close WAD file
//Args: wad_t addr
void WAD_Close(wad_t* wad);

#endif
