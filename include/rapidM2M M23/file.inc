#pragma library File

/**
 * <summary>
 *   File result codes.
 * </summary>
 */
const
{
  FILE_DISK_ERR = -1,             /* A hard error occurred in the low level disk I/O layer */
  FILE_INT_ERR = -2,              /* Assertion failed */
  FILE_NOT_READY = -3,            /* The physical drive cannot work */
  FILE_NO_FILE = -4,              /* Could not find the file */
  FILE_NO_PATH = -5,              /* Could not find the path */
  FILE_INVALID_NAME = -6,         /* The path name format is invalid */
  FILE_DENIED = -7,               /* Access denied due to prohibited access or directory full */
  FILE_EXIST = -8,                /* Access denied due to prohibited access */
  FILE_INVALID_OBJECT = -9,       /* The file/directory object is invalid */
  FILE_WRITE_PROTECTED = -10,     /* The physical drive is write protected */
  FILE_INVALID_DRIVE = -11,       /* The logical drive number is invalid */
  FILE_NOT_ENABLED = -12,         /* The volume has no work area */
  FILE_NO_FILESYSTEM = -13,       /* There is no valid FAT volume */
  FILE_MKFS_ABORTED = -14,        /* The f_mkfs() aborted due to any problem */
  FILE_TIMEOUT = -15,             /* Could not get a grant to access the volume within defined period */
  FILE_LOCKED = -16,              /* The operation is rejected according to the file sharing policy */
  FILE_NOT_ENOUGH_CORE = -17,     /* LFN working buffer could not be allocated */
  FILE_TOO_MANY_OPEN_FILES = -18, /* Number of open files > _FS_LOCK */
  FILE_INVALID_PARAMETER = -19    /* Given parameter is invalid */
};

/**
 * <summary>
 *   File access modes.
 * </summary>
 */
const
{
  FILE_IO_READ = 0,        /* file must exist */
  FILE_IO_WRITE,           /* creates a new file */
  FILE_IO_READWRITE,       /* opens an existing file, or creates a new file */
  FILE_IO_APPEND,          /* appends to file (write-only) */
};

/**
 * <summary>
 *   File positional reference.
 * </summary>
 */
const
{
  FILE_SEEK_START = 0,
  FILE_SEEK_CURRENT,
  FILE_SEEK_END,
};

/**
 * <summary>
 *   File attributes (bitmask).
 * </summary>
 */
const
{
  FILE_AM_RDO = 0x01,   /* Read only */
  FILE_AM_HID = 0x02,   /* Hidden */
  FILE_AM_SYS = 0x04,   /* System */
  FILE_AM_DIR = 0x10,   /* Directory */
  FILE_AM_ARC = 0x20,   /* Archive */
};

/**
 * <summary>
 *   End of file indication.
 * </summary>
 */
const EOF = -1;

native fsetbuf(buf{}, size);
native fopen(&File: handle, const name{}, mode = FILE_IO_READWRITE);
native fclose(File: handle);
native fremove(const name{});
native frename(const oldname{}, const newname{});
native fmkdir(const name{});
native flistdir(funcidx, const path{}={0});  // funcidx(name{}, size, fattime, mode);

native fwrite(File: handle, const buffer{}, len);
native fread(File: handle, buffer{}, len);

native fseek(File: handle, position = 0, whence = FILE_SEEK_START);
native flength(File: handle);
native fstat(const name{}, &size = 0, &fattime = 0, &mode = 0);
