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
struct tm_minute_t {
	uint16_t day;
	uint16_t min;
};


// Function declarations
//
void m2d_system_time(struct tm_minute_t *tm);
void m2d_print_time(struct tm_minute_t *tm);

#endif