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
#define PAGE_MAP_SZ		(DK_NUM_PAGES / 8)
uint8_t page_map[PAGE_MAP_SZ];


// m2d_set_page()
// Marks the specified page number as "used" (TRUE) or free (FALSE)
//
uint8_t m2d_set_page(uint16_t n, bool used)
{
	if (n >= DK_NUM_PAGES)
		error(1, 0, "Illegal page number %d in page map\n", n);

	uint8_t mask = (1 << (n % 8));
	uint8_t old = page_map[n >> 3] & mask;

	if (used)
		page_map[n >> 3] |= mask;
	else
		page_map[n >> 3] &= ~mask;

	return old;
}


// find_free_page()
// Finds the next unmarked page in the page map
//
uint16_t m2d_find_free_page()
{
	for (uint16_t i = DK_PAGE_START; i < DK_NUM_PAGES; i ++)
	{
		uint8_t p = m2d_set_page(i, true);

		if (p == 0)
			return i;
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
		uint16_t pg = bswap_16(*pt);

		if (pg != DK_NIL_PAGE)
			m2d_set_page(pg / 13, false);
			
		*pt = bswap_16(DK_NIL_PAGE);
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

		fdp = &(s.type.fd);
		if (fdp->fd_kind != bswap_16(FDK_NOFILE))
		{
			// Check for internal data mismatch
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
}