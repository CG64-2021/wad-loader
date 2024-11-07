#ifndef __WAD_H__
#define __WAD_H__

//You can find more info about WAD file here:
//https://doomwiki.org/wiki/WAD

//Error codes
#define W_FOPENERROR 1 //if fopen() fails
#define W_FTRUNCATED 2 //if the file is truncated
#define W_NONVALID 3 //if a file is not a valid wad file
#define W_LMALLOCERR 4 //If lump->data couldn't be mallocated
#define W_LUMPNOTFOUND 5 //If the desired lump was not found

typedef struct
{
	FILE* fp;
	uint32_t numlumps;
	uint32_t lumplistpos;
} wad_t;

typedef struct
{
	uint32_t size;
	uint8_t* data;
} lump_t;

//Load a WAD file and its lump list
//Don't forget to WAD_Close() if you won't use it anymore
//Args: filename, wad_t addr
//Return: 0 if success, errno if fail 
int WAD_LoadFile(const char* fn, wad_t* wad);

//Get lump data dinamically from the wad
//Don't forget to WAD_FreeLumpData() if you won't use it anymore
//Args: lumpname, wad_t addr, lump_t addr
//Return: 0 if success, errno if fail
int WAD_LoadLumpData(const char* ln, wad_t* wad, lump_t* lump);

//Print full list of lumps to screen
//Args: wad_t addr
//Return: 0 if success, errno if fail
int WAD_PrintLumpList(wad_t* wad);

//Free allocated lump data
//Args: lumpdata_t addr
void WAD_FreeLumpData(lump_t* lump);

//Close WAD file
//Args: wad_t addr
void WAD_Close(wad_t* wad);

#endif