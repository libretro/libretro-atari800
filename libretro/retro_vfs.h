#ifndef RETRO_VFS_H__
#define RETRO_VFS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "libretro.h"

void retro_set_vfs_interface(retro_environment_t environ_cb);
bool is_retro_vfs_available(void);

typedef struct retro_file retro_file_t;

retro_file_t* retro_vfs_fopen(const char *path, unsigned mode, unsigned hints);
int retro_vfs_fclose(retro_file_t *file);
int64_t retro_vfs_fsize(retro_file_t *file);
int64_t retro_vfs_ftell(retro_file_t *file);
int64_t retro_vfs_fseek(retro_file_t *file, int64_t offset, int seek_position);
int64_t retro_vfs_fread(retro_file_t *file, void *s, uint64_t len);
int64_t retro_vfs_fwrite(retro_file_t *file, const void *s, uint64_t len);
bool retro_vfs_exists(const char *path);

#endif
