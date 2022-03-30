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

#endif