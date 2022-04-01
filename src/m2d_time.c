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

#include <time.h>
#include "m2d_time.h"


// m2d_system_time()
// Assigns the current system time to the supplied variable
//
void m2d_system_time(struct tm_minute_t *tim)
{
	time_t now = time(NULL);
	struct tm *t_now = localtime(&now);

	tim->day = (t_now->tm_mday + 1) + ((t_now->tm_mon + 1) << 5)
		+ ((t_now->tm_year - 1900) << 9);

	tim->min = (t_now->tm_hour * 3600) + t_now->tm_min;
}


// m2d_print_time()
// Prints the supplied Lilith time to stdout
//
void m2d_print_time(struct tm_minute_t *tm)
{
	uint8_t t_hr = tm->min / 3600;
	uint8_t t_min = (tm->min % 3600) / 60;
	uint8_t t_day = tm->day % 32;
	uint8_t t_mon = (tm->day >> 5) % 16;
	uint16_t t_yr = (tm->day >> 9) + 1900;

	if (tm->day != 0)
		printf(
			"%02d.%02d.%4d %02d:%02d", 
			t_day, t_mon, t_yr, t_hr, t_min
		);
}