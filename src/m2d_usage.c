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


void m2d_version()
{
    fprintf(stderr, PACKAGE " v" PACKAGE_VERSION 
		" by " PACKAGE_BUGREPORT" (" VERSION_BUILD_DATE ")\n"
		"Lilith Modula-2 Machine Disk Utility\n\n"
	);
}


void m2d_usage()
{
    fprintf(stderr,
        "USAGE: " PACKAGE 
		" [-Vvlxhfic] [-d dest_dir] img_file [file_arg|files]\n\n"
        "-l\tList directory of img_file\n"
        "\tIf file_arg is omitted: list all entries\n"
        "\totherwise, list files matching regex in file_arg\n\n"
		"-c\tCreate and format new (empty) image file as img_file\n"
		"-i\tImport specified files into img_file\n"
		"-p\tList page tables of files matching file_arg\n"
        "-x\tExtract files matching file_arg from img_file\n"
        "-d\tExtract into destination 'dest_dir' (must already exist)\n\n"
        "-f\tForce mode (overwrites existing files and images)\n"
		"-t\tConvert text file EOL characters (Lilith<->Unix)\n"
        "-v\tVerbose output\n"
       "-h\tShow this help information\n"
        "-V\tShow version information\n\n"
        "img_file is the filename of a disk image of an\n"
        "original Lilith Modula-2 machine.\n\n"
    );
}