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

	// Parse command line options
	opterr = 0;
	while ((c = getopt (argc, argv, "Vvlxhfd:ic")) != -1)
	{
		switch (c)
		{
		case 'l' :
			mode = M_LISTDIR;
			break;

		case 'x' :
			mode = M_EXTRACT;
			break;

		case 'd' :
			outdir = optarg;
			break;

		case 'v' :
			verbose = true;
			break;

		case 'f' :
			force = true;
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
		if (! (imgfile_fd = fopen(imgfile, (mode == M_FORMAT) ? "w+" :"r+")))
		{
			error(1, errno, "Can't open image file '%s'", imgfile);
		}
		if (verbose)
		{
			m2d_version();
			VERBOSE("> Image file name: %s\n", imgfile)
		}
	}
	else
	{
		error(1, 0, "No image file specified.");
	}

	// Check for optional file_arg argument
	if (optind + 1 < argc)
	{
		filearg = argv[optind + 1];
	}
	if (verbose)
	{
		VERBOSE("> File argument: '%s'\n", filearg ? filearg : "*")
		if (force)
			VERBOSE("> Force mode enabled; existing files will be overwritten\n")
	}

	// Execute requested program function
	switch (mode)
	{
		case M_LISTDIR :
			// ob_listdir(imgfile_fd, filearg, brute, verbose);
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

			// m2d_extract(imgfile_fd, filearg, outdir, force, verbose);
			break;

		case M_IMPORT :
			// Import files into image
			break;

		case M_FORMAT :
			// Create new (empty) image file
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