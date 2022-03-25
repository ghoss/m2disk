//=====================================================
// m2disk
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2DISK_H
#define _M2DISK_H   1

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <config.h>


// Verbose output macro
extern bool verbose;
#define VERBOSE(...)  if (verbose) printf(__VA_ARGS__);



// Function declarations
// void ob_listdir(FILE *fd, char *filearg, bool brute, bool verbose);
// void ob_extract(
// 	FILE *fd, char *filearg, char *outdir,
// 	bool brute, bool verbose
// );
// bool ob_get_fileheader(FILE *fd, diskadr_t n, fileheader_t *hdr);
// uint32_t ob_get_filesize(fileheader_t *hdr);
// bool ob_get_sector(FILE *fd, diskadr_t n, sector_t *p);
// void ob_traverse(
// 	FILE *fd, diskadr_t n, char *filearg,
// 	void (*callproc)(direntry_t *)
// );
// void ob_traverse_brute(
// 	FILE *fd, char *filearg, void (*callproc)(direntry_t *)
// );

#endif