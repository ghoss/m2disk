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

#include <byteswap.h>
#include <time.h>
#include "m2d_time.h"


// m2d_system_time()
// Assigns the current system time to the supplied variable
//
void m2d_system_time(struct tm_minute_t *tim)
{
	time_t now = time(NULL);
	struct tm *t_now = localtime(&now);

	tim->day = bswap_16((t_now->tm_mday + 1)
		+ ((t_now->tm_mon + 1) << 5)
		+ (t_now->tm_year << 9));

	tim->min = bswap_16((t_now->tm_hour * 60) + t_now->tm_min);
}


// m2d_print_time()
// Prints the supplied Lilith time to stdout
//
void m2d_print_time(struct tm_minute_t *tm)
{
	uint16_t tmm = bswap_16(tm->min);
	uint16_t tmd = bswap_16(tm->day);

	if (tmd != 0)
	{
		uint8_t t_hr = tmm / 60;
		uint8_t t_min = tmm % 60;
		uint8_t t_day = tmd % 32;
		uint8_t t_mon = (tmd >> 5) % 16;
		uint16_t t_yr = (tmd >> 9) + 1900;

		printf(
			"%02d.%02d.%4d %02d:%02d", 
			t_day, t_mon, t_yr, t_hr, t_min
		);
	}
}