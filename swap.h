#ifndef __SWAP_H__
#define __SWAP_H__

//Wad files are stored little-endian
#ifdef __BIG_ENDIAN__
	#define swapU16(x) ((x >> 8) | (x << 8))
	#define swapU32(x) ((x >> 24) | ((x >> 8) & 0xFF00) | ((x >> 8) & 0xFF0000) | (x << 24))
#else
	#define swapU16(x) (x)
	#define swapU32(x) (x)
#endif

#endif