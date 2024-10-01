#include "atari.h"

#define NO_CART 0
#define A5200_CART 1
#define A800_CART 2



// Atari 5200 carts
#define a5200 0			// 4, 8, NS16, 32
#define a5200_40 1		// Bounty Bob CRC32: 57e7945e
#define a5200_ee_16 2   // 2 16k eeproms
#define a5200_64 3		// supercarts
#define a5200_128 4
#define a5200_256 5
#define a5200_512 6
#define a5200_40_ALT 7 // Bounty Bob CRC32: 7873c6dd

#define a5200_unsupported  	100
#define a5200_incomplete	101
#define a5200_bad_dump		102


typedef struct {
	int type;
	char name[50];
	int size;
	ULONG crc; 	
} a5200_rom;


// Atari 800 carts
#define a800 0				// STD_8, STD_16, XEGS_32
#define a800_40 1			// Bounty Bob
#define a800_WILL_64 2		// 64 KB Williams cartridge
#define a800_XE_07_64 3		// XEGS 64 KB cartridge (banks 0-7)
#define a800_XE_128 4		// XEGS 128 KB cartridge
//#define a800_XE_256 4		// XEGS 256 KB cartridge.  Not sure if any exist.
//#define a800_XE_512 5		// XEGS 512 KB cartridge.  Not sure if any exist.
//#define a800_XE_1024 6	// XEGS 1 MB cartridge.  Not sure if any exist.
#define a800_MAX_128 5		// Atarimax 128 KB Flash cartridge
#define a800_MAX_1024 6		// Atarimax 1 MB Flash cartridge (old)
#define a800_OSS_M091_16 15	// OSS one chip 16 KB cartridge
#define a800_TURBOSOFT_64 50	// Turbosoft  64 KB cartridge (Chile - Unlicensed)
#define a800_TURBOSOFT_128 51	// Turbosoft 128 KB cartridge (Chile - Unlicensed)
#define a800_TURBOSOFT_64_WILL 99	// Turbosoft  64 KB cartridge with Williams banking (Chile - Unlicensed)

#define a800_unsupported  	100
#define a800_incomplete		101
#define a800_bad_dump		102

typedef struct {
	int type;
	char name[50];
	int size;
	ULONG crc; 	
} a800_rom;

extern const a5200_rom a5200_game[];
extern const a800_rom a800_game[];

int is_5200_cart(ULONG crc);
int is_800_cart(ULONG crc);
int is_cart(ULONG crc);
