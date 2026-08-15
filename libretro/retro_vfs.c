#include "retro_vfs.h"
#include "retro_utils.h"

struct retro_file
{
   struct retro_vfs_file_handle *vfs_fp;
   FILE *fp; // fallback
};

static struct retro_vfs_interface *vfs_iface = NULL;

void retro_set_vfs_interface(retro_environment_t environ_cb)
{
   struct retro_vfs_interface_info vfs_iface_info;

   // already initialized
   if (vfs_iface)
      return;

   if (!environ_cb)
      return;

   vfs_iface_info.required_interface_version = 3;
   vfs_iface_info.iface                       = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_iface_info))
      vfs_iface = vfs_iface_info.iface;
}

bool is_retro_vfs_available(void)
{
   return vfs_iface != NULL;
}

static const char *vfs_mode_to_stdio(unsigned mode)
{
   if (mode & RETRO_VFS_FILE_ACCESS_UPDATE_EXISTING)
      return (mode & RETRO_VFS_FILE_ACCESS_WRITE) ? "r+b" : "rb";
   if ((mode & RETRO_VFS_FILE_ACCESS_READ_WRITE) == RETRO_VFS_FILE_ACCESS_READ_WRITE)
      return "w+b";
   if (mode & RETRO_VFS_FILE_ACCESS_WRITE)
      return "wb";
   return "rb";
}

retro_file_t* retro_vfs_fopen(const char *path, unsigned mode, unsigned hints)
{
   retro_file_t *file;

   if (!path || (*path == '\0'))
      return NULL;

   file = (retro_file_t*)calloc(1, sizeof(*file));
   if (!file)
      return NULL;

   if (vfs_iface && vfs_iface->open)
   {
      file->vfs_fp = vfs_iface->open(path, mode, hints);
      if (file->vfs_fp)
         return file;

      free(file);
      return NULL;
   }

   /* no VFS interface available, use plain stdio. */
   file->fp = fopen(path, vfs_mode_to_stdio(mode));
   if (!file->fp)
   {
      free(file);
      return NULL;
   }

   return file;
}

int retro_vfs_fclose(retro_file_t *file)
{
   int ret = 0;

   if (!file)
      return -1;

   if (file->vfs_fp)
      ret = vfs_iface->close(file->vfs_fp);
   else if (file->fp)
      ret = fclose(file->fp);

   free(file);
   return ret;
}

int64_t retro_vfs_fsize(retro_file_t *file)
{
   if (!file)
      return -1;

   if (file->vfs_fp)
      return vfs_iface->size(file->vfs_fp);

   if (file->fp)
   {
      long cur, size;

      cur = ftell(file->fp);
      if (cur < 0)
         return -1;
      if (fseek(file->fp, 0, SEEK_END) != 0)
         return -1;
      size = ftell(file->fp);
      fseek(file->fp, cur, SEEK_SET);
      return size;
   }

   return -1;
}

int64_t retro_vfs_ftell(retro_file_t *file)
{
   if (!file)
      return -1;

   if (file->vfs_fp)
      return vfs_iface->tell(file->vfs_fp);

   if (file->fp)
      return ftell(file->fp);

   return -1;
}

int64_t retro_vfs_fseek(retro_file_t *file, int64_t offset, int seek_position)
{
   if (!file)
      return -1;

   if (file->vfs_fp)
      return vfs_iface->seek(file->vfs_fp, offset, seek_position);

   if (file->fp)
   {
      int whence;
      switch (seek_position)
      {
         case RETRO_VFS_SEEK_POSITION_CURRENT: whence = SEEK_CUR; break;
         case RETRO_VFS_SEEK_POSITION_END:     whence = SEEK_END; break;
         default:                              whence = SEEK_SET; break;
      }
      return fseek(file->fp, (long)offset, whence);
   }

   return -1;
}

int64_t retro_vfs_fread(retro_file_t *file, void *s, uint64_t len)
{
   if (!file || !s)
      return -1;

   if (file->vfs_fp)
      return vfs_iface->read(file->vfs_fp, s, len);

   if (file->fp)
      return (int64_t)fread(s, 1, (size_t)len, file->fp);

   return -1;
}

int64_t retro_vfs_fwrite(retro_file_t *file, const void *s, uint64_t len)
{
   if (!file || !s)
      return -1;

   if (file->vfs_fp)
      return vfs_iface->write(file->vfs_fp, s, len);

   if (file->fp)
      return (int64_t)fwrite(s, 1, (size_t)len, file->fp);

   return -1;
}

bool retro_vfs_exists(const char *path)
{
   if (!path || (*path == '\0'))
      return false;

   if (vfs_iface && vfs_iface->stat)
   {
      int32_t size = 0;
      int ret = vfs_iface->stat(path, &size);
      return (ret & RETRO_VFS_STAT_IS_VALID) != 0;
   }

   /* no VFS stat callback, use the existing stdio/stat check. */
   return file_exists(path);
}
