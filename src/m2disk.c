//=====================================================
// m2disk
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include "m2disk.h"
#include "m2d_usage.h"
#include "m2d_extract.h"
#include "m2d_listdir.h"
#include "m2d_import.h"
#include "m2d_pagemap.h"
#include "m2d_medos.h"


// Global variables
bool verbose = false;

// Implemented operation modes
typedef enum {
	M_LISTDIR,
	M_EXTRACT,
	M_IMPORT,
	M_FORMAT,
	M_UNKNOWN
} mode_type;


int main(int argc, char **argv)
{
	char c;
	char *imgfile = NULL;
	FILE *imgfile_fd = NULL;
	char *outdir = NULL;
	char *filearg = NULL;
	mode_type mode = M_UNKNOWN;
	bool force = false;
	bool convert = false;

	// Parse command line options
	opterr = 0;
	while ((c = getopt (argc, argv, "Vvlxhftd:ic")) != -1)
	{
		switch (c)
		{
			case 'c' :
				mode = M_FORMAT;
				break;

			case 'l' :
				mode = M_LISTDIR;
				break;

			case 'x' :
				mode = M_EXTRACT;
				break;
			
			case 'i' :
				mode = M_IMPORT;

			case 'd' :
				outdir = optarg;
				break;

			case 'v' :
				verbose = true;
				break;

			case 'f' :
				force = true;
				break;

			case 't' :
				convert = true;
				break;

			case 'h' :
				m2d_usage();
				exit(0);

			case 'V' :
				m2d_version();
				exit(0);

			case '?' :
				error(1, 0,
					"Unrecognized option (run \"" PACKAGE " -h\" for help)."
				);
				break;

			default :
				break;
		}
	}

	// Check for image_file
	if (optind < argc)
	{
		// Get image file name
		imgfile = argv[optind];
		imgfile_fd = fopen(imgfile, "r+");
		if (mode == M_FORMAT)
		{
			// In format mode, overwrite existing files only if forced
			if ((imgfile_fd == NULL) || force)
			{
				imgfile_fd = freopen(imgfile, "w+", imgfile_fd);
			}
			else
			{
				error(1, errno, 
					"Image file '%s' exists (use -f to overwrite)",
					imgfile
				);
			}
		}
		if (imgfile_fd == NULL)
			error(1, errno, "Can't open image file '%s'", imgfile);

		if (verbose)
			m2d_version();
		VERBOSE("> Image file name: %s\n", imgfile)
	}
	else
	{
		error(1, 0, "No image file specified.");
	}

	// Check for optional file_arg argument
	if ((mode == M_EXTRACT) || (mode == M_LISTDIR))
	{
		if (optind + 1 < argc)
			filearg = argv[optind + 1];

		VERBOSE("> File argument: '%s'\n", filearg ? filearg : "*")
	}

	if (force)
		VERBOSE("> Force mode enabled; existing files will be overwritten\n")
	
	// Execute requested program function
	switch (mode)
	{
		case M_LISTDIR :
			m2d_listdir(imgfile_fd, filearg);
			VERBOSE("\n")
			break;

		case M_EXTRACT :
			// Check if output directory exists and change to it	
			if ((outdir != NULL) && (*outdir != '\0'))
			{
				if (chdir(outdir) != 0)
					error(1, errno, "Invalid output directory '%s'", outdir);
			}
			if (verbose)
				VERBOSE("> Destination dir: '%s'\n", outdir ? outdir : ".")
			if (convert)
				VERBOSE("> Text file conversion enabled\n")
			VERBOSE("\n")

			m2d_extract(imgfile_fd, filearg, force, convert);
			break;

		case M_IMPORT : {
			// Import files into image
			uint16_t ok = 0;
			if (convert)
				VERBOSE("> Text file conversion enabled\n")

			// Load pagemap since we must find unused sectors
			m2d_load_pagemap(imgfile_fd);

			// Run through each source file argument
			for (uint16_t j = optind + 1; j < argc; j ++)
			{
				if (m2d_import(imgfile_fd, argv[j], force, convert))
					ok ++;
			}
			if (ok == 0)
				VERBOSE("> No files imported.\n")
			VERBOSE("\n")
			break;
		}

		case M_FORMAT :
			// Create new (empty) image file
			if (m2d_init_image(imgfile_fd))
			{
				VERBOSE("> Image file created successfully.\n")
			}
			else
			{
				error(0, errno, "Can't create image file");
			}
			break;

		default :
			error(0, 0, 
				"Unknown or no function specified"
				" (run \"" PACKAGE " -h\" for help)."
			);
			break;
	}

	// Close image file
	fclose(imgfile_fd);
	return 0;
}