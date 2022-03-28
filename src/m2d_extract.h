//=====================================================
// m2d_extract.h
// Lilith image to Unix file export function.
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2D_EXTRACT_H
#define _M2D_EXTRACT_H   1

#include "m2disk.h"


// Forward declarations
//
void m2d_extract(FILE *f, char *filearg, bool force, bool convert);

#endif