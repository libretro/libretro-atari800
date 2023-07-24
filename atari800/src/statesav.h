#ifndef STATESAV_H_
#define STATESAV_H_

#include "atari.h"

int StateSav_SaveAtariState(const char *filename, const char *mode, UBYTE SaveVerbose);
int StateSav_ReadAtariState(const char *filename, const char *mode);

void StateSav_SaveUBYTE(const UBYTE *data, int num);
void StateSav_SaveUWORD(const UWORD *data, int num);
void StateSav_SaveINT(const int *data, int num);
void StateSav_SaveFNAME(const char *filename);

void StateSav_ReadUBYTE(UBYTE *data, int num);
void StateSav_ReadUWORD(UWORD *data, int num);
void StateSav_ReadINT(int *data, int num);
void StateSav_ReadFNAME(char *filename);

#if defined(__LIBRETRO__)
int Retro_SaveAtariState(uint8_t* data, size_t size, UBYTE SaveVerbose);
int Retro_ReadAtariState(const uint8_t* data, size_t size);

void Retro_SaveUBYTE(const UBYTE* data, int num);
void Retro_SaveUWORD(const UWORD* data, int num);
void Retro_SaveINT(const int* data, int num);
void Retro_SaveFNAME(const char* filename);

void Retro_ReadUBYTE(UBYTE* data, int num);
void Retro_ReadUWORD(UWORD* data, int num);
void Retro_ReadINT(int* data, int num);
void Retro_ReadFNAME(char* filename);
#endif

#endif /* STATESAV_H_ */
