#ifndef CRC32_H_
#define CRC32_H_

#include <stdio.h>

#include "atari.h"

#ifdef __LIBRETRO__
#include "retro_vfs.h"
#endif

/* Compute CRC32 of LEN bytes from BUF. CRC should be set initially to
   0xffffffff. */
#ifdef HAVE_LIBZ
#include <zlib.h>
#define CRC32_Update(crc, buf, len) (crc32((crc) ^ 0xFFFFFFFF, (buf), (len)) ^ 0xFFFFFFFF)
#else
ULONG CRC32_Update(ULONG crc, UBYTE const *buf, unsigned int len);
#endif

/* Compute CRC32 of a stream F and store it at *RESULT. Return non-zero on
   success or 0 on read error. */
#ifndef __LIBRETRO__
int CRC32_FromFile(FILE *f, ULONG *result);
#else
int CRC32_FromFile(retro_file_t *f, ULONG *result);
#endif

#endif /* CRC32_H_ */
