//=====================================================
// m2d_time.c
// System time functions
//
// Lilith Machine Disk Utility
//
// Guido Hoss, 25.03.2022
//
// Published by Guido Hoss under GNU Public License V3.
//=====================================================

#include "m2d_time.h"


// get_system_time()
// Assigns the current system time to the supplied variable
//
void get_system_time(struct tm_minute_t *tm)
{
	tm->day = 0;
	tm->min = 0;
}