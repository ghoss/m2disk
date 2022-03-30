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

#include "m2d_medos.h"
#include "m2d_dir.h"
#include "m2d_extract.h"


// Forward declarations
//
void m2d_extract(FILE *f, char *filearg, bool force, bool convert)
{
	void extract_file(dir_entry_t *d)
	{
		VERBOSE("file '%s'\n", d->name)
	};

	// Check all directory entries for match with "filearg"
	m2d_traverse(f, filearg, &extract_file);
}