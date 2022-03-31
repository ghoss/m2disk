//=====================================================
// m2d_extract.c
// Lilith image to Unix file export function.
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include <byteswap.h>
#include "m2d_medos.h"
#include "m2d_dir.h"
#include "m2d_extract.h"


// Forward declarations
//
void m2d_extract(FILE *f, char *filearg, bool force, bool convert)
{
	bool extract_file(dir_entry_t *d)
	{
		VERBOSE("%s (%d bytes)... ", d->name, d->len)
		uint32_t len = d->len;
		uint16_t i = 0;

		// Page entry / 13 = actual page address
		// (see SEK Medos-2 filesystem thesis p.74)
		uint16_t page = (bswap_16(d->page_tab[i]) / 13) * 8;

		// Open target file
		FILE *of = fopen(d->name, "r");
		if (of != NULL)
		{
			fclose(of);
			if (! force)
				error(1, 0, "File exists (use -f)");
		}
		if ((of = fopen(d->name, "w")) == NULL)
			error(1, errno, "Can't create file");

		// Loop through each page entry (1 page = 8 sectors)
		while ((len > 0) && (i < M2D_PAGETAB_LEN)
			&& ((page != DK_NIL_PAGE)))
		{
			// Determine number of used sectors
			uint16_t max_sec = ((len > 8 * DK_SECTOR_SZ) 
				? 8 : (1 + len / DK_SECTOR_SZ));

			// Loop through each used sector
			for (uint16_t j = 0; j < max_sec; j ++)
			{
				// Read sector from image
				struct disk_sector_t s;
				m2d_read_sector(f, &s, page + j);

				// Find number of used bytes in this sector
				uint16_t max_byte = (len > DK_SECTOR_SZ)
					? DK_SECTOR_SZ : len;

				if (max_byte > 0)
				{
					// Perform optional text conversion
					if (convert)
						m2d_text_convert(&s, max_byte, true);

					// Write the correct number of bytes to destination
					if (fwrite(&s, max_byte, 1, of) != 1)
					{
						error(1, errno, 
							"Can't write %d bytes to '%s'", 
							max_byte, d->name
						);
					}
					len -= max_byte;
				}
			}
			page = (bswap_16(d->page_tab[++ i]) / 13) * 8;
		}
		if (len != 0)
			error(0, 0, "File length mismatch in '%s'", d->name);
		
		fclose(of);
		VERBOSE("OK\n")

		return true;
	};

	// Check all directory entries for match with "filearg"
	m2d_traverse(f, filearg, extract_file);
}