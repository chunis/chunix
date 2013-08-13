#ifndef __TIME_H__
#define __TIME_H__

#include <types.h>

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

struct timespec {
	time_t   tv_sec;
	uint32_t tv_nsec;
};

#endif
