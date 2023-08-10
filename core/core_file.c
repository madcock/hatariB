#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "../hatari/src/includes/main.h"
#include "../libretro/libretro.h"
#include "core.h"
#include "core_internal.h"

#define MAX_PATH          2048
#define MAX_FILENAME      256
#define MAX_SYSTEM_FILE   128
#define MAX_SYSTEM_DIR     16

// set to 0 to only use standard filesystem
#define USE_RETRO_VFS   1

static int sf_count = 0;
static char sf_filename[MAX_SYSTEM_FILE][MAX_FILENAME];
static int sf_dir_count = 0;
static char sf_dirname[MAX_SYSTEM_DIR][MAX_FILENAME];
static char sf_dirlabel[MAX_SYSTEM_DIR][MAX_FILENAME];

struct retro_vfs_interface* retro_vfs = NULL;
int retro_vfs_version = 0;

static char system_path[MAX_PATH] = "";
static char save_path[MAX_PATH] = "";
static bool save_path_ready = false;

//
// Utilities
//

// safe truncating strcpy/strcat
void strcpy_trunc(char* dest, const char* src, unsigned int len)
{
	while (len > 1 && *src)
	{
		*dest = *src;
		++dest; ++src;
		--len;
	}
	*dest = 0;
}
void strcat_trunc(char* dest, const char* src, unsigned int len)
{
	while (len > 1 && *dest)
	{
		++dest; --len;
	}
	strcpy_trunc(dest, src, len);
}

//
// Basic file system abstraction
//

static char temp_fn[MAX_PATH];

const char* temp_fn2(const char* path1, const char* path2)
{
	if(path1) strcpy_trunc(temp_fn, path1, sizeof(temp_fn));
	else temp_fn[0] = 0;
	if(path2) strcat_trunc(temp_fn, path2, sizeof(temp_fn));
	return temp_fn;
}

const char* temp_fn3(const char* path1, const char* path2, const char* path3)
{
	temp_fn2(path1,path2);
	if(path3) strcat_trunc(temp_fn, path3, sizeof(temp_fn));
	return temp_fn;
}

const char* get_temp_fn()
{
	return temp_fn;
}

void save_path_init()
{
	const char* cp;
	if (save_path_ready) return;
	if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY,(void*)&cp) && cp)
	{
		retro_log(RETRO_LOG_DEBUG,"GET_SAVE_DIRECTORY success\n");
		strcpy_trunc(save_path, cp, sizeof(save_path));
		if (strlen(save_path) > 0)
			strcat_trunc(save_path, "/", sizeof(save_path));
	}
	else
	{
		save_path[0] = 0;
	}
	retro_log(RETRO_LOG_INFO,"save_path: %s\n",save_path);
	save_path_ready = true;
}

uint8_t* core_read_file(const char* filename, unsigned int* size_out)
{
	uint8_t* d = NULL;
	unsigned int size = 0;
	retro_log(RETRO_LOG_INFO,"core_read_file('%s')\n",filename);

	if (size_out) *size_out = 0;
	if (retro_vfs_version >= 3)
	{
		int64_t rs;
		struct retro_vfs_file_handle* f = retro_vfs->open(filename,RETRO_VFS_FILE_ACCESS_READ,0);
		if (f == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_read_file (VFS) not found: %s\n",filename);
			return NULL;
		}
		rs = retro_vfs->size(f);
		if (rs < 0)
		{
			retro_log(RETRO_LOG_ERROR,"core_read_file (VFS) size error: %s\n",filename);
			return NULL;
		}
		size = (unsigned int)rs;
		d = malloc(size);
		if (d == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_read_file (VFS) out of memory: %s\n",filename);
			retro_vfs->close(f);
			return NULL;
		}
		retro_vfs->read(f,d,size);
		retro_vfs->close(f);
	}
	else
	{
		FILE* f = fopen(filename,"rb");
		if (f == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_read_file not found: %s\n",filename);
			return NULL;
		}
		fseek(f,0,SEEK_END);
		size = (unsigned int)ftell(f);
		fseek(f,0,SEEK_SET);
		d = malloc(size);
		if (d == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_read_file out of memory: %s\n",filename);
			fclose(f);
			return NULL;
		}
		fread(d,1,size,f);
		fclose(f);
	}
	if (size_out) *size_out = size;
	return d;
}

bool core_write_file(const char* filename, unsigned int size, const uint8_t* data)
{
	retro_log(RETRO_LOG_INFO,"core_write_file('%s',%d)\n",filename,size);
	if (retro_vfs_version >= 3)
	{
		struct retro_vfs_file_handle* f = retro_vfs->open(filename,RETRO_VFS_FILE_ACCESS_WRITE,0);
		if (f == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_write_file (VFS) could not open: %s\n",filename);
			return false;
		}
		if (retro_vfs->write(f,data,size) < 0)
		{
			retro_log(RETRO_LOG_ERROR,"core_write_file (VFS) could not write: %s\n",filename);
			retro_vfs->close(f);
			return false;
		}
		retro_vfs->close(f);
	}
	else
	{
		FILE* f = fopen(filename,"wb");
		if (f == NULL)
		{
			retro_log(RETRO_LOG_ERROR,"core_write_file could not open: %s\n",filename);
			return false;
		}
		if (size != fwrite(data,1,size,f))
		{
			retro_log(RETRO_LOG_ERROR,"core_write_file could not write: %s\n",filename);
			fclose(f);
			return false;
		}
		fclose(f);
	}
	return true;
}

uint8_t* core_read_file_system(const char* filename, unsigned int* size_out)
{
	//retro_log(RETRO_LOG_INFO,"core_read_file_system('%s')\n",filename);
	return core_read_file(temp_fn2(system_path,filename),size_out);
}

uint8_t* core_read_file_save(const char* filename, unsigned int* size_out)
{
	//retro_log(RETRO_LOG_INFO,"core_read_file_save('%s')\n",filename);
	save_path_init();
	return core_read_file(temp_fn2(save_path,filename),size_out);
}

bool core_write_file_save(const char* filename, unsigned int size, const uint8_t* data)
{
	//retro_log(RETRO_LOG_INFO,"core_write_file_save('%s',%d)\n",filename,size);
	save_path_init();
	return core_write_file(temp_fn2(save_path,filename), size, data);
}

//
// Direct file system abstraction
//

void* core_file_open(const char* path, int access)
{
	retro_log(RETRO_LOG_INFO,"core_file_open('%s',%d)\n",path,access);
	void* handle = NULL;
	if (retro_vfs_version >= 3)
	{
		struct retro_vfs_file_handle* f;
		unsigned mode = RETRO_VFS_FILE_ACCESS_READ;
		if      (access == CORE_FILE_WRITE   ) mode = RETRO_VFS_FILE_ACCESS_WRITE;
		else if (access == CORE_FILE_REVISE  ) mode = RETRO_VFS_FILE_ACCESS_READ_WRITE | RETRO_VFS_FILE_ACCESS_UPDATE_EXISTING;
		else if (access == CORE_FILE_TRUNCATE) mode = RETRO_VFS_FILE_ACCESS_READ_WRITE;
		f = retro_vfs->open(path,mode,0);
		handle = (void*)f;
	}
	else
	{
		FILE* f;
		const char* mode = "rb";
		if      (access == CORE_FILE_WRITE   ) mode = "wb";
		else if (access == CORE_FILE_REVISE  ) mode = "rb+";
		else if (access == CORE_FILE_TRUNCATE) mode = "wb+";
		f = fopen(path,mode);
		handle = (void*)f;
	}
	retro_log(RETRO_LOG_DEBUG,"core_file_open('%s',%d) = %p\n",path,access,handle);
	return handle;
}

void* core_file_open_system(const char* path, int access)
{
	return core_file_open(temp_fn2(system_path,path),access);
}

void* core_file_open_save(const char* path, int access)
{
	save_path_init();
	return core_file_open(temp_fn2(save_path,path),access);
}

bool core_file_exists(const char* path)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_exists('%s')\n",path);
	if (retro_vfs_version >= 3)
	{
		int vst = retro_vfs->stat(path,NULL);
		if (!(vst & RETRO_VFS_STAT_IS_VALID)) return false;
		if (vst & RETRO_VFS_STAT_IS_DIRECTORY) return false;
		return true;
	}
	else
	{
		// conditions based on hatari/src/file.c File_Exists
		struct stat fs;
		if ((0 == stat(path, &fs)) && (fs.st_mode & (S_IRUSR|S_IWUSR)) && !S_ISDIR(fs.st_mode))
		{
			return true;
		}
	}
	return false;
}

bool core_file_save_exists(const char* filename)
{
	save_path_init();
	return core_file_exists(temp_fn2(save_path,filename));
}


void core_file_close(void* file)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_close(%p)\n",file);
	if (retro_vfs_version >= 3)
	{
		retro_vfs->close((struct retro_vfs_file_handle*)file);
	}
	else
	{
		fclose((FILE*)file);
	}
}

int core_file_seek(void* file, int64_t offset, int dir)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_seek(%p,%d,%d)\n",file,(int)offset,dir);
	if (retro_vfs_version >= 3)
	{
		int mode = RETRO_VFS_SEEK_POSITION_START;
		if      (dir == SEEK_CUR) mode = RETRO_VFS_SEEK_POSITION_CURRENT;
		else if (dir == SEEK_END) mode = RETRO_VFS_SEEK_POSITION_END;

		if (retro_vfs->seek((struct retro_vfs_file_handle*)file,offset,mode) < 0)
			return -1;
		else
			return 0;
	}
	else
	{
		return fseek((FILE*)file,(long)offset,dir);
	}
}

int64_t core_file_read(void* buf, int64_t len, void* file)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_read(%p,%d,%p)\n",buf,len,file);
	if (retro_vfs_version >= 3)
	{
		int64_t result = retro_vfs->read((struct retro_vfs_file_handle*)file,buf,len);
		if (result < 0) return 0;
		return result;
	}
	else
	{
		return fread(buf,1,len,(FILE*)file);
	}
}

int64_t core_file_write(const void* buf, int64_t len, void* file)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_write(%p,%d,%p)\n",buf,len,file);
	if (retro_vfs_version >= 3)
	{
		int64_t result = retro_vfs->write((struct retro_vfs_file_handle*)file,buf,len);
		if (result < 0) return 0;
		return result;
	}
	else
	{
		return fwrite(buf,1,len,(FILE*)file);
	}
}

int core_file_flush(void* file)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_flush(%p)\n",file);
	if (retro_vfs_version >= 3)
	{
		return retro_vfs->flush((struct retro_vfs_file_handle*)file);
	}
	else
	{
		return fflush((FILE*)file);
	}
}

int core_file_remove(const char* path)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_remove('%s')\n",path);
	if (retro_vfs_version >= 3)
	{
		return retro_vfs->remove(path);
	}
	else
	{
		return remove(path);
	}
}

int core_file_rename(const char* old_path, const char* new_path)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_rename('%s','%s')\n",old_path,new_path);
	if (retro_vfs_version >= 3)
	{
		return retro_vfs->rename(old_path, new_path);
	}
	else
	{
		return rename(old_path, new_path);
	}
}

int core_file_stat(const char* path, struct stat* fs)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_stat('%s',%p)\n",path,fs);
	if (retro_vfs_version >= 3)
	{
		int32_t vsize;
		int vst = retro_vfs->stat(path,&vsize);
		if (!(vst & RETRO_VFS_STAT_IS_VALID)) return -1;
		if (fs)
		{
			memset(fs,0,sizeof(struct stat));
			// gemdos.c uses:
			//   st_mtime
			//   st_atime
			//   st_mode with: S_IWURS, S_IFDIR, and S_ISDIR
			//   st_size
			fs->st_mtime = 0; // don't have this
			fs->st_atime = 0; // don't have this
			fs->st_mode |= S_IWUSR; // don't have this, assume write permissions always
			if (vst & RETRO_VFS_STAT_IS_DIRECTORY)
			{
			    fs->st_mode |= S_IFDIR;
				fs->st_mode |= S_ISDIR(~0);
			}
			if (vst & RETRO_VFS_STAT_IS_CHARACTER_SPECIAL) fs->st_mode |= S_IFCHR; // not used but retro vfs has it
			fs->st_size = (off_t)vsize;
		}
		return 0;
	}
	else
	{
		return stat(path, fs);
	}
}

void* core_file_opendir(const char* path)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_opendir('%s')\n",path);
	if (retro_vfs_version >= 3)
	{
		return (void*)retro_vfs->opendir(path,true);
	}
	else
	{
		return (void*)opendir(path);
	}
}

struct dirent* core_file_readdir(void* dir)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_readdir('%p')\n",dir);
	if (retro_vfs_version >= 3)
	{
		static struct dirent d;
		memset(&d,0,sizeof(d));
		// gemdos.c only uses name
		if (retro_vfs->readdir((struct retro_vfs_dir_handle*)dir))
		{
			const char* name = retro_vfs->dirent_get_name((struct retro_vfs_dir_handle*)dir);
			if (name) strcpy_trunc(d.d_name,name,sizeof(d.d_name));
		}
		return &d;
	}
	else
	{
		return readdir((DIR*)dir);
	}
}

int core_file_closedir(void* dir)
{
	retro_log(RETRO_LOG_DEBUG,"core_file_closedir('%p')\n",dir);
	if (retro_vfs_version >= 3)
	{
		return retro_vfs->closedir((struct retro_vfs_dir_handle*)dir);
	}
	else
	{
		return closedir((DIR*)dir);
	}
}

//
// Setup and system file scan
//

static void core_file_system_add(const char* filename, bool prefix_hatarib)
{
	if (sf_count >= MAX_SYSTEM_FILE) return;
	sf_filename[sf_count][0] = 0;
	if (prefix_hatarib)
		strcpy_trunc(sf_filename[sf_count],"hatarib/",MAX_FILENAME);
	strcat_trunc(sf_filename[sf_count],filename,MAX_FILENAME);
	//retro_log(RETRO_LOG_INFO,"core_file_system_add: %s\n",sf_filename[sf_count]);
	++sf_count;
}

static void core_file_system_add_dir(const char* filename)
{
	if (sf_dir_count >= MAX_SYSTEM_DIR) return;
	if (!strcmp(filename,".")) return;
	if (!strcmp(filename,"..")) return;
	strcpy_trunc(sf_filename[sf_count],"hatarib/",MAX_FILENAME);
	strcat_trunc(sf_dirname[sf_dir_count],filename,MAX_FILENAME);
	strcpy_trunc(sf_dirlabel[sf_dir_count],filename,MAX_FILENAME);
	strcat_trunc(sf_dirlabel[sf_dir_count],"/",MAX_FILENAME);
	++sf_dir_count;
}

void core_file_set_environment(retro_environment_t cb)
{
	struct retro_vfs_interface_info retro_vfs_info = { 3, NULL };
	const char* cp;
	
	sf_count = 0;
	sf_dir_count = 0;
	memset(sf_filename,0,sizeof(sf_filename));
	memset(sf_dirname,0,sizeof(sf_dirname));
	memset(sf_dirlabel,0,sizeof(sf_dirlabel));

#if USE_RETRO_VFS
	if (retro_vfs_version > 0)
	{
		// if already initialized RETRO_ENVIRONMENT_GET_VFS_INTERFACE seems to fail,
		// so just keep what we've got.
		retro_log(RETRO_LOG_INFO,"retro_vfs version: %d\n",retro_vfs_version);
	}
	else if (cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE,(void*)&retro_vfs_info))
	{
		retro_vfs_version = retro_vfs_info.required_interface_version;
		retro_vfs = retro_vfs_info.iface;
		retro_log(RETRO_LOG_INFO,"retro_vfs version: %d\n",retro_vfs_version);
	}
	else
#endif
	{
		(void)retro_vfs_info;
		retro_log(RETRO_LOG_INFO,"retro_vfs not available\n");
	}

	// system path is available immediately
	if (cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY,(void*)&cp) && cp)
	{
		retro_log(RETRO_LOG_DEBUG,"GET_SYSTEM_DIRECTORY success\n");
		strcpy_trunc(system_path, cp, sizeof(system_path));
		if (strlen(system_path) > 0)
			strcat_trunc(system_path, "/", sizeof(system_path));
	}
	retro_log(RETRO_LOG_INFO,"system_path: %s\n",system_path);

	// save path is not ready until retro_load_game
	save_path_ready = false;

	// scan system folder
	if (retro_vfs_version >= 3) // retro_vfs
	{
		struct retro_vfs_file_handle* fh;
		struct retro_vfs_dir_handle* dir;

		// check for tos.img

		fh = retro_vfs->open(temp_fn2(system_path,"tos.img"),RETRO_VFS_FILE_ACCESS_READ,0);
		if (fh)
		{
			core_file_system_add("tos.img",false);
			retro_vfs->close(fh);
		}

		// scan system/hatarib/
		dir = retro_vfs->opendir(temp_fn2(system_path,"hatarib"),false);
		if (dir)
		{
			while(retro_vfs->readdir(dir))
			{
				const char* fn = retro_vfs->dirent_get_name(dir);
				if (fn)
				{
					if (!retro_vfs->dirent_is_dir(dir))
						core_file_system_add(fn,true);
					else
						core_file_system_add_dir(fn);
				}
			}
			retro_vfs->closedir(dir);
		}
	}
	else // posix stat/opendir/readdir/closedir
	{
		DIR* dir;
		struct dirent* de;
		struct stat fs;

		// check for tos.img
		if((0 == stat(temp_fn2(system_path,"tos.img"), &fs)) && !(fs.st_mode & S_IFDIR))
		{
			core_file_system_add("tos.img",false);
		}

		// scan system/hatarib/
		dir = opendir(temp_fn2(system_path,"hatarib"));
		if (dir)
		{
			while ((de = readdir(dir)))
			{
				if(0 == stat(temp_fn3(system_path,"hatarib/",de->d_name), &fs))
				{
					 if (!(fs.st_mode & S_IFDIR))
						core_file_system_add(de->d_name,true);
					else
						core_file_system_add_dir(de->d_name);
				}
			}
			closedir(dir);
		}
	}
}

int core_file_system_count()
{
	return sf_count;
}

const char* core_file_system_filename(int index)
{
	if (index >= sf_count) return "";
	return sf_filename[index];
}

int core_file_system_dir_count()
{
	return sf_dir_count;
}

const char* core_file_system_dirname(int index)
{
	if (index >= sf_dir_count) return "";
	return sf_dirname[index];
}

const char* core_file_system_dirlabel(int index)
{
	if (index >= sf_dir_count) return "";
	return sf_dirlabel[index];
}
