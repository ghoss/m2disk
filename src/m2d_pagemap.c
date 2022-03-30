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

#include "m2d_medos.h"
#include "m2d_pagemap.h"


// Static page map data table
uint8_t page_map[DK_NUM_SECTORS / 8];


// set_page()
// Marks the specified page number as "used"
//
void m2d_set_page(FILE *f, uint16_t n)
{

}


// alloc_page()
// Finds the next free page and marks it as "used"
// Returns the page number
//
uint16_t m2d_alloc_page(FILE *f)
{
	return 0;
}


// load_pagemap()
// Calculates the free page map of the specified image file
//
void m2d_load_pagemap(FILE *f)
{

}