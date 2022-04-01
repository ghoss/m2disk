//=====================================================
// m2d_listdir.c
// High-level directory listing
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <byteswap.h>
#include "m2d_dir.h"
#include "m2d_listdir.h"


// m2d_list_dir()
// List filesystem directory with optional wildcard filter
//
void m2d_list_dir(FILE *f, char *filearg)
{
	// Callback to print a directory entry
	bool print_dir(dir_entry_t *d)
	{
		printf(
			"%c%c %-26.26s%4d%9d  ",
			(d->reserved != 0) ? '*' : ' ',
			(d->protected != 0) ? 'R' : ' ',
			d->name,
			d->filenum,
			d->len
		);
		
		m2d_print_time(&(d->mtime));

		printf("\n");
		return true;
	};

	// Traverse the directory tree starting at its root
	m2d_traverse(f, filearg, print_dir);
}


// m2d_list_pagetab()
// List page table of specified file(s)
//
void m2d_list_pagetab(FILE *f, char *filearg)
{
	bool print_pagetab(dir_entry_t *d)
	{
		uint16_t i;

		printf("%s:", d->name);
		for (i = 0; i < M2D_PAGETAB_LEN; i ++)
		{
			uint16_t pg = bswap_16(d->page_tab[i]);
			if (pg == DK_NIL_PAGE)
				break;

			if ((i % 8) == 0)
				printf("\n");

			printf("  %05d", pg / 13);
		}
		printf("\n");
		return true;
	};

	// Traverse the directory tree starting at its root
	m2d_traverse(f, filearg, print_pagetab);
}
