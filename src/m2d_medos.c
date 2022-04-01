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
	uint16_t sectors;	// Number of sectors in file
};

const struct reserved_file_t reserved_file[DK_NUM_RESFILES] = 
{
	{ "FS.FileDirectory",		DK_DIR_START,	DK_NUM_FILES },
	{ "FS.FileDirectory.Back",	36768,			DK_NUM_FILES },
	{ "FS.NameDirectory",		DK_NAME_START, 	DK_NAMEDIR_LEN },
	{ "FS.NameDirectory.Back",	37536,			DK_NAMEDIR_LEN },
	{ "FS.BadPages",			0,				0 },
	{ "PC.BootFile",			0,				192 },
	{ "PC.BootFile.Back",		1248,			192 },
	{ "PC.DumpFile",			192,			512 },
	{ "PC.Dump1File",			704,			512 }
};


// text_convert()
// Converts all line endings in the given sector
//
#define M2_EOL	'\036'
#define UX_EOL	'\n'

void m2d_text_convert(struct disk_sector_t *s, uint16_t n, bool to_unix)
{
	uint8_t *p = s->type.b;

	for (uint16_t i = 0; i < n; i ++, p ++)
	{
		if (*p == M2_EOL)
		{
			if (to_unix) *p = UX_EOL;
		}
		else if (*p == UX_EOL)
		{
			if (! to_unix) *p = M2_EOL;
		}
	}
}


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


// m2d_write_sector()
// Writes sector number n to disk
//
bool m2d_write_sector(FILE *f, struct disk_sector_t *s, uint16_t n)
{
	n = calc_image_sector(n);

	bool res = (fseek(f, n * DK_SECTOR_SZ, SEEK_SET) != -1)
		&& (fwrite(s, DK_SECTOR_SZ, 1, f) == 1);

	if (! res)
		error(0, errno, "write_sector(%d) failed", n);
	return res;
}


// m2d_read_sector()
// Reads sector number n from disk
//
bool m2d_read_sector(FILE *f, struct disk_sector_t *s, uint16_t n)
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
		if (! m2d_write_sector(f, &s, i))
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
	fdp->fd_kind = bswap_16(FDK_NOFILE);

	// Initialize filler area
	bzero(fdp->fdk.filler, M2D_MAX_FILLER + 1);

	// Initialize page table
	for (uint16_t i = 0; i < M2D_PAGETAB_LEN; i ++)
		fdp->page_tab[i] = bswap_16(DK_NIL_PAGE);

	// Write empty file directory to disk
	for (uint16_t i = 0; i < DK_NUM_FILES; i ++)
	{
		fdp->file_num = bswap_16(i);
		if (! m2d_write_sector(f, &s, DK_DIR_START + i))
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
		struct name_desc_t *ndp = &(s.type.nd[i]);

		memset(ndp->en, ' ', M2D_EXTNAME_LEN);
		ndp->nd_kind = NDK_FREE;
		ndp->file_num = 0;
		ndp->version = 0;
		ndp->fres = 0;
	}

	// Write empty name directory to disk
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		if (! m2d_write_sector(f, &s, DK_NAME_START + i))
			return false;
	}
	VERBOSE("Created empty name directory: OK\n")
	return true;
}


// make_filedir_entry()
// Makes a new file directory entry for the specified file
//
bool make_filedir_entry(
	FILE *f,
	uint16_t fnum, uint32_t sz, 
	uint16_t *pt, bool reserved
) {
	struct disk_sector_t s;

	uint16_t sn = DK_DIR_START + fnum;
	if (! m2d_read_sector(f, &s, sn))
		return false;

	struct file_desc_t *fdp = &s.type.fd;
	fdp->fd_kind = bswap_16(FDK_FATHER);
	fdp->file_num = bswap_16(fnum);
	fdp->version = UINT16_MAX;
	
	// Set file size
	struct fd_father_t *fa = &fdp->fdk.father;
	fa->len.sectors = bswap_16(sz / DK_SECTOR_SZ);
	fa->len.bytes = bswap_16(sz % DK_SECTOR_SZ);

	// Set file flags
	fa->ref_flag = bswap_16(1);
	fa->mod_flag = 0;
	fa->prot_flag = fdp->reserved 
		= bswap_16(reserved ? 1 : 0);

	// Set file creation and modification times
	m2d_system_time(&fa->ctime);
	m2d_system_time(&fa->mtime);

	// Copy page table
	memcpy(fdp->page_tab, pt, M2D_PAGETAB_LEN * sizeof(uint16_t));

	// Clear son table
	for (uint16_t j = 0; j < M2D_MAX_SONS - 1; j ++)
		fa->sontab[j] = bswap_16(DK_NIL_PAGE);

	// Write directory entry to disk
	if (! m2d_write_sector(f, &s, sn))
		return false;

	return true;
}


// make_namedir_entry()
// Makes a new name director entry for the specified file
//
bool make_namedir_entry(FILE *f, char *fname, uint16_t fnum)
{
	// Convert null-terminated string to space-padded string
	void convert_filename(char *m2f, char *uxf)
	{
		bool space = false;

		for (uint16_t i = 0; i < M2D_EXTNAME_LEN; i ++)
		{
			if (*uxf == '\0')
				space = true;
			*m2f = space ? ' ' : *uxf;
			m2f ++;
			uxf ++;
		}
	}

	struct disk_sector_t s;

	uint16_t nsn = DK_NAME_START + (fnum / DK_NUM_ND_SECT);
	if (! m2d_read_sector(f, &s, nsn))
		return false;

	struct name_desc_t *ndp = &s.type.nd[fnum % DK_NUM_ND_SECT];

	convert_filename(&(ndp->en[0]), fname);
	ndp->nd_kind = bswap_16(NDK_FNAME);
	ndp->file_num = bswap_16(fnum);
	ndp->version = UINT16_MAX;

	// Write name directory entry to disk
	if (! m2d_write_sector(f, &s, nsn))
		return false;

	return true;
}


// m2d_register_file()
// Creates entries for a new file in the file and name
// directories
//
bool m2d_register_file(
	FILE *f, char *fname,
	uint16_t fnum, uint32_t sz, 
	uint16_t *pt, bool reserved
) {
	return make_filedir_entry(f, fnum, sz, pt, reserved)
		&& make_namedir_entry(f, fname, fnum);
}


// init_reserved_files()
// Initialize the reserved file entries
//
bool init_reserved_files(FILE *f)
{
	// Part 1: Make directory entry
	for (uint16_t i = 0; i < DK_NUM_RESFILES; i ++)
	{
		uint16_t pt[M2D_PAGETAB_LEN];

		uint16_t pages = (reserved_file[i].sectors + 7) / 8;
		uint16_t start = reserved_file[i].start / 8;

		// Fill continuous page table
		for (uint16_t j = 0; j < M2D_PAGETAB_LEN; j ++)
		{
			pt[j] = bswap_16((j < pages) ?
				((start + j) * 13) : DK_NIL_PAGE);
		}

		bool res = m2d_register_file(
			f, reserved_file[i].en,
			i, reserved_file[i].sectors * DK_SECTOR_SZ, 
			pt, true);
			
		if (! res) 
			return false;
	}
	return true;
}


// init_image_file()
// Creates an empty image file with an initialized directory and
// the standard default files.
//
bool m2d_init_image(FILE *f)
{
	return init_disk_space(f)
		&& init_file_dir(f)
		&& init_name_dir(f)
		&& init_reserved_files(f);
}