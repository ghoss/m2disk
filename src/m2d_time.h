//=====================================================
// m2d_time.h
// System time functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#ifndef _M2D_TIME_H
#define _M2D_TIME_H   1

#include "m2disk.h"


// Lilith time data type
typedef struct {
	uint16_t day;
	uint16_t min;
} tm_minute_t;


// Function declarations
//
void get_system_time(tm_minute_t *tm);

#endif