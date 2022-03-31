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

#include <string.h>
#include "m2d_dir.h"
#include "m2d_pagemap.h"
#include "m2d_import.h"


// m2d_import()
// Imports "infile" into the opened Lilith image f.
// Returns TRUE if successful.
//
bool m2d_import(FILE *f, char *infile, bool force, bool convert)
{
	dir_entry_t d;
	FILE *infile_fd;
	
	// Check if filename is too long
	if (strlen(infile) > M2D_EXTNAME_LEN)
	{
		error(0, 0, "Filename '%s' too long, ignored", infile);
		return false;
	}

	// Search for filename in image file directory
	if (m2d_lookup_file(f, infile, &d))
	{
		// File found; file number in d.filenum
		if (! force)
		{
			error(0, 0, "File '%s' already exists in image (use -f)", infile);
			return false;
		}
		
		VERBOSE("File found, i=%d\n", d.filenum)
	}
	else
	{
		// File NOT found; first free directory entry in d.filenum
		VERBOSE("NOT found, first free=%d\n", d.filenum)
	}

	// Open input file
	if (((infile_fd = fopen(infile, "r"))) == NULL)
	{
		error(0, errno, "Can't open '%s'", infile);
		return false;
	}

	fclose (infile_fd);
	return true;
}