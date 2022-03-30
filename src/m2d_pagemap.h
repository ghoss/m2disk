//=====================================================
// m2d_pagemap.h
// Disk pagemap table
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2D_PAGEMAP_H
#define _M2D_PAGEMAP_H   1

#include "m2disk.h"


// Forward declarations
//
void m2d_set_page(FILE *f, uint16_t n);
uint16_t m2d_alloc_page(FILE *f);
void m2d_load_pagemap(FILE *f);

#endif