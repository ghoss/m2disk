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
#include <fnmatch.h>
#include <byteswap.h>
#include "m2d_dir.h"


// m2d_traverse()
// Traverse directory
//
void m2d_traverse(FILE *f, char *filearg, bool (*callproc)(dir_entry_t *))
{
	struct disk_sector_t s;

	// Scan all sectors in name directory
	for (uint16_t i = 0; i < DK_NAMEDIR_LEN; i ++)
	{
		if (! m2d_read_sector(f, &s, DK_NAME_START + i))
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
				strncpy(d.name, ndp->en, M2D_EXTNAME_LEN);
				for (int16_t k = M2D_EXTNAME_LEN; k >= 0; k --)
				{
					if (d.name[k] == ' ')
						d.name[k] = '\0';
				}

				// Check if filename pattern matches
				if ((filearg != NULL) 
					&& (fnmatch(filearg, d.name, FNM_PATHNAME) != 0))
					continue;

				// Load associated file descriptor from disk
				d.filenum = bswap_16(ndp->file_num);

				struct disk_sector_t s1;
				if (! m2d_read_sector(f, &s1, DK_DIR_START + (i * 8 + j)))
					break;

				// Copy page table
				struct file_desc_t *fdp = &s1.type.fd;
				memcpy(d.page_tab, fdp->page_tab, sizeof(d.page_tab));

				// Set remaining file info from file descriptor
				d.reserved = bswap_16(fdp->reserved);
				if (d.filenum != bswap_16(fdp->file_num))
					error(1, 0, 
						"Directory entry mismatch (file# %d)", d.filenum
					);

				struct fd_father_t *fa = &s1.type.fd.fdk.father;
				d.len = bswap_16(fa->len.sectors) * DK_SECTOR_SZ 
					+ bswap_16(fa->len.bytes);

				d.mtime = bswap_16(fa->mtime.day);
				d.ctime = fa->ctime.day;
				d.protected = bswap_16(fa->prot_flag);

				// Callback procedure
				callproc(&d);
			}
		}
	}
}


// m2d_lookup_file()
// Checks for the directory entry with specified filename
// Returns TRUE and the directory entry if file found.
// Returns FALSE and the index of the first free directory
// entry in d.idx if file not found
//
bool m2d_lookup_file(FILE *f, char *fn, dir_entry_t *d)
{
	int16_t curr_idx = -1;
	int16_t first_free = -1;
	bool found = false;

	bool check_entry(dir_entry_t *dt)
	{
		if (strcmp(dt->name, fn) == 0)
		{
			// Entry exists; copy its information to caller
			memcpy(d, dt, sizeof(dir_entry_t));
			found = true;
			return false;
		}

		// If not found, check for free gaps in file# sequence
		if (dt->filenum > curr_idx + 1)
		{
			if (first_free == -1)
				first_free = curr_idx + 1;
		}
		curr_idx = dt->filenum;
		return true;
	}

	// Scan all directory entries
	m2d_traverse(f, NULL, check_entry);

	// If not found, report first free directory entry
	if (! found)
	{
		d->reserved = bswap_16(0);
		
		if (first_free != -1)
			d->filenum = first_free;
		else if (curr_idx < DK_NUM_FILES - 1)
			d->filenum = curr_idx;
		else
			error(1, 0, "Directory full");
	}

	return found;
}