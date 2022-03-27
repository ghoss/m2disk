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
	struct disk_sector_t s;

	// Scan all sectors in name directory
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		if (! read_sector(f, &s, DK_NAME_START + i))
			break;

		// Scan name entries in each sector
		for (uint16_t j = 0; j < DK_NUM_ND_SECT; j ++)
		{
			struct name_desc_t *ndp = &s.type.nd[j];

			// Skip free entries
			if (ndp->nd_kind == bswap_16(NDK_FNAME))
			{
				dir_entry_t d;

				// Make null-terminated filename
				bzero(d.name, M2D_EXTNAME_LEN + 1);
				strncpy(d.name, ndp->en, M2D_EXTNAME_LEN);

				// Load associated file descriptor from disk
				d.filenum = bswap_16(ndp->file_num);

				struct disk_sector_t s1;
				if (! read_sector(f, &s1, DK_DIR_START + d.filenum))
					break;

				// Set remaining file info from file descriptor
				struct file_desc_t *fdp = &s1.type.fd;
				d.reserved = bswap_16(fdp->reserved);

				struct fd_father_t *fa = &s1.type.fd.fdk.father;
				d.len = bswap_16(fa->len.block) * DK_SECTOR_SZ 
					+ bswap_16(fa->len.byte);

				d.mtime = bswap_16(fa->mtime.day);
				d.ctime = fa->ctime.day;
				d.protected = bswap_16(fa->prot_flag);

				// Callback procedure
				callproc(&d);
			}
		}
	}
}
