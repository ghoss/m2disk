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

// Time
typedef struct {
	uint16_t day;
	uint16_t min;
} fd_minute_t;

// File descriptor kind
#define FDK_NOFILE	0
#define FDK_FATHER	1
#define FDK_SON		2

// File Descriptor
typedef struct {
	uint16_t unused;
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
			fd_minute_t ctime;
			fd_minute_t mtime;
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
		if (fwrite(&s, DK_SECTOR_SZ, 1, f) != 1)
			return false;
	}
	return true;
}


// init_file_dir()
// Initializes an empty file directory
//
bool init_file_dir(FILE *f)
{
	return true;
}


// init_name_dir()
// Initializes an empty name directory
//
bool init_name_dir(FILE *f)
{
	return true;
}


// init_reserved_files()
// Initialize and copy the reserved files
//
bool init_reserved_files(FILE *f)
{
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