//=====================================================
// m2d_medos.h
// Disk structure definitions and functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2D_MEDOS_H
#define _M2D_MEDOS_H   1

#include "m2disk.h"
#include "m2d_time.h"


// Disk structure definitions
//
#define M2D_MAX_FILLER		27
#define M2D_MAX_SONS		16
#define M2D_PAGETAB_LEN		96
#define M2D_EXTNAME_LEN		24

// Position
struct file_pos_t {
	uint16_t block;
	uint16_t byte;
};

// File descriptor kind
#define FDK_NOFILE	0
#define FDK_FATHER	1
#define FDK_SON		2

// File Descriptor
struct file_desc_t {
	uint16_t reserved;
	uint16_t file_num;	// File number
	uint16_t version;
	uint16_t fd_kind;	// File descriptor kind
	union
	{
		uint16_t filler[M2D_MAX_FILLER + 1];
		struct {
			struct file_pos_t len;
			uint16_t mod_flag;
			uint16_t ref_flag;
			uint16_t prot_flag;
			struct tm_minute_t ctime;
			struct tm_minute_t mtime;
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
};

// Name descriptor
#define NDK_FREE	0
#define NDK_FNAME	1
#define tt 043200
#define tt1 044600
#define tt2 0140

struct name_desc_t {
	char en[M2D_EXTNAME_LEN];
	uint16_t nd_kind;
	uint16_t file_num;
	uint16_t version;
	uint16_t fres;
};

// Disk dimensions
#define DK_SECTOR_SZ	256		// Size of a sector in bytes
#define DK_NUM_SECTORS	37632	// Number of sectors on disk
#define DK_NUM_FILES	768		// Max. number of files on disk
#define DK_NUM_ND_SECT	(DK_SECTOR_SZ / sizeof(struct name_desc_t))
#define DK_NIL_PAGE		61152	// Value of the NIL page pointer

// Disk sector
struct disk_sector_t {
	union {
		uint8_t b[DK_SECTOR_SZ];
		struct file_desc_t fd;
		struct name_desc_t nd[DK_NUM_ND_SECT];
	} type;
};

// Special file locations
#define DK_DIR_START	18048	// 1st file directory sector
#define DK_NAME_START	18816	// 1st name directory sector
#define DK_NAMEDIR_LEN	(DK_NUM_FILES / DK_NUM_ND_SECT)


// Function declarations
//
bool init_image_file(FILE *f);
bool write_sector(FILE *f, struct disk_sector_t *s, uint16_t n);
bool read_sector(FILE *f, struct disk_sector_t *s, uint16_t n);

#endif