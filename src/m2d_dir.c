//=====================================================
// m2d_dir.c
// Low-level directory listing and handling functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <string.h>
#include <byteswap.h>
#include "m2d_dir.h"


// m2d_traverse()
// Traverse directory
//
void m2d_traverse(FILE *f, char *filearg, void (*callproc)(dir_entry_t *))
{
	disk_sector_t s;

	// Scan all sectors in name directory
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		if (! read_sector(f, &s, DK_NAME_START + i))
			break;

		// Scan name entries in each sector
		for (uint16_t j = 0; j < DK_NUM_ND_SECT; j ++)
		{
			name_desc_t *ndp = &(s.type.nd[j]);

			// Skip free entries
			if (ndp->nd_kind == bswap_16(NDK_FNAME))
			{
				dir_entry_t d;

				// Make null-terminated filename
				bzero(&d.name, M2D_EXTNAME_LEN + 1);
				strncpy(&(d.name[0]), &(ndp->en[0]), M2D_EXTNAME_LEN);

				// Set remaining file info
				d.reserved = 0;
				d.len = 0;
				d.mtime = 0;

				// Callback procedure
				callproc(&d);
			}
		}
	}
}
