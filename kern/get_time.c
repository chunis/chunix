//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#include <x86.h>
#include <string.h>
#include <types.h>
#include <time.h>


#define CMOS_ADDRESS  0x70
#define CMOS_DATA     0x71
#define CURRENT_YEAR  2013     // Change this each year

#define bcd2bin(a) (((a) & 0x0F) + ((a) / 16 * 10))

int century_register = 0x00;   // get from ACPI table if possible
struct tm sys_tm;  // system time

static int get_update_in_progress_flag(void)
{
	outb(CMOS_ADDRESS, 0x0A);
	return (inb(CMOS_DATA) & 0x80);
}

static uint8_t get_RTC_register(int reg)
{
	outb(CMOS_ADDRESS, reg);
	return inb(CMOS_DATA);
}

// read registers until get the same values twice in a row
// to avoid getting dodgy/inconsistent values due to RTC updates
void read_rtc(struct tm *tmp)
{
	struct tm oldtm;
	unsigned char century;
	unsigned char last_century;
	unsigned char registerB;

	// Make sure an update isn't in progress
	while (get_update_in_progress_flag())
		;  // do nothing

	tmp->tm_sec = get_RTC_register(0x00);
	tmp->tm_min = get_RTC_register(0x02);
	tmp->tm_hour = get_RTC_register(0x04);
	tmp->tm_mday = get_RTC_register(0x07);
	tmp->tm_mon = get_RTC_register(0x08);
	tmp->tm_year = get_RTC_register(0x09);
	if(century_register != 0) {
		century = get_RTC_register(century_register);
	}

	do {
		oldtm.tm_sec = tmp->tm_sec;
		oldtm.tm_min = tmp->tm_min;
		oldtm.tm_hour = tmp->tm_hour;
		oldtm.tm_mday = tmp->tm_mday;
		oldtm.tm_mon = tmp->tm_mon;
		oldtm.tm_year = tmp->tm_year;
		last_century = century;

		while (get_update_in_progress_flag())
			;  // do nothing

		tmp->tm_sec = get_RTC_register(0x00);
		tmp->tm_min = get_RTC_register(0x02);
		tmp->tm_hour = get_RTC_register(0x04);
		tmp->tm_mday = get_RTC_register(0x07);
		tmp->tm_mon = get_RTC_register(0x08);
		tmp->tm_year = get_RTC_register(0x09);
		if(century_register != 0) {
			century = get_RTC_register(century_register);
		}
	} while( (oldtm.tm_sec == tmp->tm_sec)
			&& (oldtm.tm_min == tmp->tm_min)
			&& (oldtm.tm_hour == tmp->tm_hour)
			&& (oldtm.tm_mday == tmp->tm_mday)
			&& (oldtm.tm_mon == tmp->tm_mon)
			&& (oldtm.tm_year == tmp->tm_year)
			&& (last_century == century) );

	// Convert BCD to binary values if necessary
	registerB = get_RTC_register(0x0B);
	if (!(registerB & 0x04)) {
		tmp->tm_sec = bcd2bin(tmp->tm_sec);
		tmp->tm_min = bcd2bin(tmp->tm_min);
		tmp->tm_hour = (( (tmp->tm_hour & 0x0F)
				+ (((tmp->tm_hour & 0x70) / 16) * 10) )
				| (tmp->tm_hour & 0x80));
		tmp->tm_mday = bcd2bin(tmp->tm_mday);
		tmp->tm_mon = bcd2bin(tmp->tm_mon);
		tmp->tm_year = bcd2bin(tmp->tm_year);
		if(century_register != 0) {
			century = bcd2bin(century);
		}
	}

	// Convert 12 hour clock to 24 hour clock if necessary
	if (!(registerB & 0x02) && (tmp->tm_hour & 0x80)) {
		tmp->tm_hour = ((tmp->tm_hour & 0x7F) + 12) % 24;
	}

	// Calculate the full (4-digit) year
	if(century_register != 0) {
		tmp->tm_year += century * 100;
	} else {
		tmp->tm_year += (CURRENT_YEAR / 100) * 100;
		if(tmp->tm_year < CURRENT_YEAR)
			tmp->tm_year += 100;
	}
}

void get_time(void)
{
	int x, y;

	read_rtc(&sys_tm);
	settextcolor(12, 0);
	get_cursor(&x, &y);
	set_cursor(30, 0);
	printk("Boot Time: %d-%d-%d %d:%d:%d",
			sys_tm.tm_year, sys_tm.tm_mon,
			sys_tm.tm_mday, sys_tm.tm_hour,
			sys_tm.tm_min, sys_tm.tm_sec);
	set_cursor(x, y);
	resettextcolor();
}
