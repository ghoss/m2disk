//=====================================================
// m2d_medos.c
// Disk structure definitions and functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <string.h>
#include "m2d_time.h"
#include "m2d_medos.h"


// Disk structure definitions
//
#define M2D_MAX_FILLER		27
#define M2D_MAX_SONS		16
#define M2D_PAGETAB_LEN		96
#define M2D_EXTNAME_LEN		24

// Position
typedef struct {
	uint16_t block;
	uint16_t byte;
} file_pos_t;

// File descriptor kind
#define FDK_NOFILE	0
#define FDK_FATHER	1
#define FDK_SON		2

// File Descriptor
typedef struct {
	uint16_t reserved;
	uint16_t file_num;	// File number
	uint16_t version;
	uint16_t fd_kind;	// File descriptor kind
	union
	{
		uint16_t filler[M2D_MAX_FILLER + 1];
		struct {
			file_pos_t len;
			uint16_t mod_flag;
			uint16_t ref_flag;
			uint16_t prot_flag;
			tm_minute_t ctime;
			tm_minute_t mtime;
			uint16_t fres[4];
			uint16_t sontab[M2D_MAX_SONS - 1];
		} father;
		struct {
			uint16_t father_num;
			uint16_t father_vers;
			uint16_t son_num;
		} son;
	} fdk;
	uint16_t page_tab[M2D_PAGETAB_LEN];
} file_desc_t;

// Name descriptor
#define NDK_FREE	0
#define NDK_FNAME	1

typedef struct {
	char en[M2D_EXTNAME_LEN];
	uint16_t nd_kind;
	uint16_t file_num;
	uint16_t version;
	uint16_t fres;
} name_desc_t;

// Disk dimensions
#define DK_SECTOR_SZ	256		// Size of a sector in bytes
#define DK_NUM_SECTORS	37632	// Number of sectors on disk
#define DK_NUM_FILES	768		// Max. number of files on disk
#define DK_NUM_ND_SECT	(DK_SECTOR_SZ / sizeof(name_desc_t))
#define DK_NIL_PAGE		61152	// Value of the NIL page pointer

// Disk sector
typedef struct {
	union {
		uint8_t b[DK_SECTOR_SZ];
		file_desc_t fd;
		name_desc_t nd[DK_NUM_ND_SECT];
	} type;
} disk_sector_t;

// Special file locations
#define DK_DIR_START	2256	// 1st file directory sector
#define DK_NAME_START	2352	// 1st name directory sector
#define DK_NAMEDIR_LEN	(DK_NUM_FILES / DK_NUM_ND_SECT)

// Reserved file entries
#define DK_NUM_RESFILES	9

typedef struct {
	char en[M2D_EXTNAME_LEN];	// File name
	uint16_t start;				// 1st sector number
	uint16_t blocks;			// Number of sectors in file
} reserved_file_t;

const reserved_file_t reserved_file[DK_NUM_RESFILES] = 
{
	{ "FS.FileDirectory        ",	DK_DIR_START,	DK_NUM_FILES },
	{ "FS.FileDirectory.Back   ",	4596,			DK_NUM_FILES },
	{ "FS.NameDirectory        ",	DK_NAME_START, 	DK_NAMEDIR_LEN },
	{ "FS.NameDirectory.Back   ",	4692,			DK_NAMEDIR_LEN },
	{ "FS.BadPages             ",	0,				0 },
	{ "PC.BootFile             ",	0,				192 },
	{ "PC.BootFile.Back        ",	156,			192 },
	{ "PC.DumpFile             ",	24,				512 },
	{ "PC.Dump1File            ",	88,				512 }
};


// write_sector()
// Writes sector number n to disk
//
bool write_sector(FILE *f, disk_sector_t *s, uint16_t n)
{
	return (fwrite(&s, DK_SECTOR_SZ, 1, f) == 1);
}


// read_sector()
// Reads sector number n from disk
//
bool read_sector(FILE *f, disk_sector_t *s, uint16_t n)
{
	return (fread(&s, DK_SECTOR_SZ, 1, f) == 1);
}


// init_disk_space()
// Creates the sectors for an empty disk
//
bool init_disk_space(FILE *f)
{
	disk_sector_t s;

	// Create a zero sector and fill the disk with it
	bzero(&s, DK_SECTOR_SZ);
	for (uint16_t i = 0; i < DK_NUM_SECTORS; i ++)
	{
		if (! write_sector(f, &s, i))
			return false;
	}
	return true;
}


// init_file_dir()
// Initializes an empty file directory
//
bool init_file_dir(FILE *f)
{
	disk_sector_t s;
	file_desc_t *fdp = &(s.type.fd);

	// Make template for an empty file directory sector
	fdp->reserved = 0;
	fdp->version = UINT16_MAX;
	fdp->fd_kind = FDK_NOFILE;

	// Initialize filler area
	bzero(&(fdp->fdk.filler), M2D_MAX_FILLER + 1);

	// Initialize page table
	for (uint16_t i = 0; i < M2D_PAGETAB_LEN; i ++)
		fdp->page_tab[i] = DK_NIL_PAGE;

	// Write empty file directory to disk
	for (uint16_t i = 0; i < DK_NUM_FILES; i ++)
	{
		fdp->file_num = i;
		if (! write_sector(f, &s, DK_DIR_START + i))
			return false;
	}
	return true;
}


// init_name_dir()
// Initializes an empty name directory
//
bool init_name_dir(FILE *f)
{
	disk_sector_t s;

	// Make template for an empty name directory sector
	for (uint16_t i = 0; i < DK_NUM_ND_SECT; i ++)
	{
		name_desc_t *ndp = &(s.type.nd[i]);

		memset(&(ndp->en), ' ', M2D_EXTNAME_LEN);
		ndp->nd_kind = 0;
		ndp->file_num = 0;
		ndp->version = 0;
		ndp->fres = 0;
	}

	// Write empty name directory to disk
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		if (! write_sector(f, &s, DK_NAME_START + i))
			return false;
	}
	return true;
}


// init_reserved_files()
// Initialize the reserved file entries
//
bool init_reserved_files(FILE *f)
{
	disk_sector_t s;
	file_desc_t *fdp = &(s.type.fd);

	for (uint16_t i = 0; i < DK_NUM_RESFILES; i ++)
	{
		// Part 1: Make directory entry
		if (! read_sector(f, &s, DK_DIR_START + i))
			return false;

		fdp->fd_kind = 1;
		fdp->fdk.father.len.block = reserved_file[i].blocks;
		fdp->fdk.father.ref_flag = 1;
		get_system_time(&fdp->fdk.father.ctime);
		get_system_time(&fdp->fdk.father.mtime);

		uint16_t blocks = (reserved_file[i].blocks + 7) / 8;
		uint16_t start = reserved_file[i].start;

		for (uint16_t j = 0; j < blocks; j ++)
			fdp->page_tab[j] = (start + j) * 13;

		for (uint16_t j = 0; j < M2D_MAX_SONS - 1; j ++)
			fdp->fdk.father.sontab[j] = DK_NIL_PAGE;

		// Write directory entry to disk
		if (! write_sector(f, &s, DK_DIR_START + i))
			return false;

		// Part 2: Make name directory entry
		if (! read_sector(f, &s, (DK_NAME_START + i) / DK_NUM_ND_SECT))
			return false;

		name_desc_t *ndp = &(s.type.nd[i % DK_NUM_ND_SECT]);
		memcpy(&(ndp->en[0]), &(reserved_file[i].en[0]), M2D_EXTNAME_LEN);
		ndp->nd_kind = 1;
		ndp->file_num = i;
		ndp->version = DK_NIL_PAGE;

		// Write name directory entry to disk
		if (! write_sector(f, &s, (DK_NAME_START + i) / DK_NUM_ND_SECT))
			return false;
	}
	return true;
}


// init_image_file()
// Creates an empty image file with an initialized directory and
// the standard default files.
//
bool init_image_file(FILE *f)
{
	return init_disk_space(f)
		&& init_file_dir(f)
		&& init_name_dir(f)
		&& init_reserved_files(f);
}