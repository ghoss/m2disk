//=====================================================
// m2d_pagemap.h
// Disk pagemap table
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <byteswap.h>
#include "m2d_medos.h"
#include "m2d_pagemap.h"


// Static page map data table
uint8_t page_map[DK_NUM_SECTORS / 8];


// m2d_set_page()
// Marks the specified page number as "used" (TRUE) or free (FALSE)
//
void m2d_set_page(uint16_t n, bool used)
{
	if (n >= DK_NUM_SECTORS)
		error(1, 0, "Illegal sector number %d in page map\n", n);

	uint16_t mask = (1 << (n % 8));

	if (used)
		page_map[n >> 3] |= mask;
	else
		page_map[n >> 3] &= ~mask;
}


// find_free_page()
// Finds the next unmarked page in the page map
//
uint16_t m2d_find_free_page()
{
	for (uint16_t i = 0; i < DK_NUM_SECTORS / 8; i ++)
	{
		uint8_t p = page_map[i];
		uint16_t pn = 0;
		while ((pn < 8) && (p != 0) && (p & 1))
		{
			pn ++;
			p >>= 1;
		}
		if (pn < 8)
		{
			pn += (i << 3);
			m2d_set_page(pn, true);
			return pn;
		}
	}
	error(1, 0, "Disk image full");
	return 0;
}


// m2d_free_pages()
// Frees all pages in the supplied page table
//
void m2d_free_pages(uint16_t *pt)
{
	for (uint16_t i = 0; i < M2D_PAGETAB_LEN; i ++)
	{
		m2d_set_page(bswap_16(*pt / 13), false);
		*pt = DK_NIL_PAGE;
		pt ++;
	}
}


// load_pagemap()
// Calculates the free page map of the specified image file
//
void m2d_load_pagemap(FILE *f)
{
	for (uint16_t i = 0; i < DK_NUM_FILES; i ++)
	{
		struct disk_sector_t s;
		struct file_desc_t *fdp;

		// Load directory sector
		if (! m2d_read_sector(f, &s, DK_DIR_START + i))
			error(1, 0, "Can't build pagemap from image");

		// Check for internal data mismatch
		fdp = &(s.type.fd);
		if (bswap_16(fdp->file_num) != i)
			error(1, 0, "File number mismatch in image directory");

		// Add all used pages of this file to the page map
		for (uint16_t j = 0; j < M2D_PAGETAB_LEN; j ++)
		{
			uint16_t p = bswap_16(fdp->page_tab[j]);

			if (p != DK_NIL_PAGE)
				m2d_set_page(p / 13, true);
			else
				break;
		}
	}
}