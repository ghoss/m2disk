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

#include "m2d_dir.h"
#include "m2d_listdir.h"


// Callback to print a directory entry
//
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


// m2d_listdir
// List filesystem directory with optional wildcard filter
//
void m2d_listdir(FILE *f, char *filearg)
{

	// Traverse the directory tree starting at its root
	m2d_traverse(f, filearg, print_dir);
}