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
uint16_t m2d_find_free_page();
void m2d_free_pages(uint16_t *pt);
void m2d_load_pagemap(FILE *f);

#endif