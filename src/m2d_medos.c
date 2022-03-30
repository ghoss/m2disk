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
#include <byteswap.h>
#include "m2d_medos.h"


// Reserved file entries
#define DK_NUM_RESFILES	9

struct reserved_file_t {
	char *en;			// File name
	uint16_t start;		// 1st sector number
	uint16_t blocks;	// Number of sectors in file
};

const struct reserved_file_t reserved_file[DK_NUM_RESFILES] = 
{
	{ "FS.FileDirectory        ",	DK_DIR_START,	DK_NUM_FILES },
	{ "FS.FileDirectory.Back   ",	36768,			DK_NUM_FILES },
	{ "FS.NameDirectory        ",	DK_NAME_START, 	DK_NAMEDIR_LEN },
	{ "FS.NameDirectory.Back   ",	37536,			DK_NAMEDIR_LEN },
	{ "FS.BadPages             ",	0,				0 },
	{ "PC.BootFile             ",	0,				192 },
	{ "PC.BootFile.Back        ",	1248,			192 },
	{ "PC.DumpFile             ",	192,			512 },
	{ "PC.Dump1File            ",	704,			512 }
};


// calc_image_sector()
// Calculate the actual disk sector in the image from a
// given logical (sequential) sector number
//
#define N_TRACKS	96
#define N_SECTORS	48
#define N_HEADS		2

uint16_t calc_image_sector(uint16_t n)
{
	uint16_t c, h, s;	// Cylinder, head, sector

	c = n / N_TRACKS;
	h = (n / N_SECTORS ) % N_HEADS;
	s = (n % N_SECTORS) * ((c < 15) ? 3 : 12);
	s = (s % N_SECTORS) + (s / N_SECTORS);

	return (c * N_TRACKS) + (h * N_SECTORS) + s;   
}


// write_sector()
// Writes sector number n to disk
//
bool write_sector(FILE *f, struct disk_sector_t *s, uint16_t n)
{
	n = calc_image_sector(n);

	bool res = (fseek(f, n * DK_SECTOR_SZ, SEEK_SET) != -1)
		&& (fwrite(s, DK_SECTOR_SZ, 1, f) == 1);

	if (! res)
		error(0, errno, "write_sector(%d) failed", n);
	return res;
}


// read_sector()
// Reads sector number n from disk
//
bool read_sector(FILE *f, struct disk_sector_t *s, uint16_t n)
{
	n = calc_image_sector(n);
	
	bool res = (fseek(f, n * DK_SECTOR_SZ, SEEK_SET) != -1)
		&& (fread(s, DK_SECTOR_SZ, 1, f) == 1);
		
	if (! res)
		error(0, errno, "read_sector(%d) failed", n);
	return res;
}


// init_disk_space()
// Creates the sectors for an empty disk
//
bool init_disk_space(FILE *f)
{
	struct disk_sector_t s;

	// Create a zero sector and fill the disk with it
	bzero(&s, DK_SECTOR_SZ);
	for (uint16_t i = 0; i < DK_NUM_SECTORS; i ++)
	{
		if (! write_sector(f, &s, i))
			return false;
	}
	VERBOSE("Created empty image file: OK\n")
	return true;
}


// init_file_dir()
// Initializes an empty file directory
//
bool init_file_dir(FILE *f)
{
	struct disk_sector_t s;

	// Make template for an empty file directory sector
	struct file_desc_t *fdp = &s.type.fd;
	fdp->reserved = 0;
	fdp->version = UINT16_MAX;
	fdp->fd_kind = FDK_NOFILE;

	// Initialize filler area
	bzero(fdp->fdk.filler, M2D_MAX_FILLER + 1);

	// Initialize page table
	for (uint16_t i = 0; i < M2D_PAGETAB_LEN; i ++)
		fdp->page_tab[i] = bswap_16(DK_NIL_PAGE);

	// Write empty file directory to disk
	for (uint16_t i = 0; i < DK_NUM_FILES; i ++)
	{
		fdp->file_num = bswap_16(i);
		if (! write_sector(f, &s, DK_DIR_START + i))
			return false;
	}
	VERBOSE("Created empty file directory: OK\n")
	return true;
}


// init_name_dir()
// Initializes an empty name directory
//
bool init_name_dir(FILE *f)
{
	struct disk_sector_t s;

	// Make template for an empty name directory sector
	for (uint16_t i = 0; i < DK_NUM_ND_SECT; i ++)
	{
		struct name_desc_t *ndp = &s.type.nd[i];

		memset(ndp->en, ' ', M2D_EXTNAME_LEN);
		ndp->nd_kind = NDK_FREE;
		ndp->version = 0;
		ndp->fres = 0;
	}

	// Write empty name directory to disk
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		for (uint16_t j = 0; j < DK_NUM_ND_SECT; j ++)
		{
			uint16_t sn = i * DK_NUM_ND_SECT + j;
			s.type.nd[j].file_num = bswap_16(sn);
		}

		if (! write_sector(f, &s, DK_NAME_START + i))
			return false;
	}
	VERBOSE("Created empty name directory: OK\n")
	return true;
}


// init_reserved_files()
// Initialize the reserved file entries
//
bool init_reserved_files(FILE *f)
{
	struct disk_sector_t s;

	// Part 1: Make directory entry
	for (uint16_t i = 0; i < DK_NUM_RESFILES; i ++)
	{
		uint16_t sn = DK_DIR_START + i;
		if (! read_sector(f, &s, sn))
			return false;

		struct file_desc_t *fdp = &s.type.fd;
		fdp->fd_kind = bswap_16(FDK_FATHER);
		struct fd_father_t *fa = &fdp->fdk.father;
		fa->len.block = bswap_16(reserved_file[i].blocks);
		fa->len.byte = 0;
		fa->ref_flag = fa->prot_flag = fdp->reserved = bswap_16(1);
		get_system_time(&fa->ctime);
		get_system_time(&fa->mtime);

		uint16_t blocks = (reserved_file[i].blocks + 7) / 8;
		uint16_t start = reserved_file[i].start;

		for (uint16_t j = 0; j < blocks; j ++)
			fdp->page_tab[j] = bswap_16((start + j) * 13);

		for (uint16_t j = 0; j < M2D_MAX_SONS - 1; j ++)
			fa->sontab[j] = bswap_16(DK_NIL_PAGE);

		// Write directory entry to disk
		if (! write_sector(f, &s, sn))
			return false;
	}

	// Part 2: Make name directory entry
	uint16_t nsn = 0;
	for (uint16_t i = 0; i < DK_NUM_RESFILES; i ++)
	{
		uint16_t nsn1 = DK_NAME_START + (i / DK_NUM_ND_SECT);
		if (nsn1 != nsn)
		{
			if (! read_sector(f, &s, nsn1))
				return false;
			nsn = nsn1;
		}

		struct name_desc_t *ndp = &s.type.nd[i % DK_NUM_ND_SECT];
		memcpy(ndp->en, reserved_file[i].en, M2D_EXTNAME_LEN);
		ndp->nd_kind = bswap_16(NDK_FNAME);
		ndp->version = bswap_16(DK_NIL_PAGE);

		// Write name directory entry to disk
		if (! write_sector(f, &s, nsn1))
			return false;

		VERBOSE("Created reserved file: %24s\n", ndp->en)
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