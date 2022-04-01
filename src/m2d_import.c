//=====================================================
// m2d_import.c
// Unix file to Lilith image import function.
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <libgen.h>
#include <string.h>
#include <byteswap.h>
#include "m2d_dir.h"
#include "m2d_pagemap.h"
#include "m2d_medos.h"
#include "m2d_import.h"


// m2d_import()
// Imports "infile" into the opened Lilith image f.
// Returns TRUE if successful.
//
bool m2d_import(FILE *f, char *infile, bool force, bool convert)
{
	dir_entry_t d;
	FILE *infile_fd;

	// Open input file
	if (((infile_fd = fopen(infile, "r"))) == NULL)
	{
		error(0, errno, "Can't open '%s'", infile);
		return false;
	}

	// Establish base name of input file
	char *bname = basename(infile);
	VERBOSE("%s... ", bname)
	
	// Check if filename is too long
	if (strlen(bname) > M2D_EXTNAME_LEN)
	{
		error(0, 0, "Filename '%s' too long, ignored", infile);
		fclose(infile_fd);
		return false;
	}

	// Search for filename in image file directory
	if (m2d_lookup_file(f, bname, &d))
	{
		// File found; file number in d.filenum
		if (! (d.reserved || force))
		{
			error(0, 0, "File '%s' already exists (use -f)", bname);
			fclose(infile_fd);
			return false;
		}

		// Deallocate any preexisting pages in directory entry
		if (! d.reserved)
			m2d_free_pages(d.page_tab);		
	}

	// Read file in pages of 8 sectors
	uint16_t page_n = 0;
	uint16_t start = 0;
	uint16_t sect = 0;
	uint32_t total = 0;
	uint16_t rd;
	struct disk_sector_t s;

	while ((rd = fread(&s, 1, DK_SECTOR_SZ, infile_fd)) > 0)
	{
		total += rd;
		if ((sect % 8) == 0)
		{
			sect = 0;
			if (d.reserved)
			{
				start = (bswap_16(d.page_tab[page_n ++]) / 13) * 8;
			}
			else if (page_n < M2D_PAGETAB_LEN)
			{
				start = m2d_find_free_page();
				d.page_tab[page_n ++] = bswap_16(start * 13);
				start *= 8;
			}
			else
			{
				error(0, 0, "File truncated (too large)");
				break;
			}
		}

		// Optional text conversion
		if (convert)
			m2d_text_convert(&s, rd, false);

		// Write sector to image
		if (m2d_write_sector(f, &s, start + sect))
			sect ++;
		else
			error(1, errno, "Can't write to image");
	}

	// Fill rest of page table
	for (uint16_t j = page_n; j < M2D_PAGETAB_LEN; j ++)
		d.page_tab[j] = bswap_16(DK_NIL_PAGE);

	// Register file in directory
	if (! m2d_register_file(
		f, bname, d.filenum, total, d.page_tab, d.reserved
	)) {
		error(0, 0, "Can't create directory entry");
	}

	fclose (infile_fd);
	VERBOSE("OK\n")
	return true;
}