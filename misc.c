/****************************************************************************
 * Miscellaneous functions.
 ****************************************************************************/

#include "main.h"

unsigned long pageSize;

/****************************************************************************
 * Get the page size.
 ****************************************************************************/
void GetPageSize() {
	pageSize = getpagesize();
}

/****************************************************************************
 * Print an integer with a suffix if it takes more than 4 digits.
 ****************************************************************************/
void PrintInteger(unsigned long number) {
	if(number < 10000L) {
		printf("%4lu ", number);
	} else if(number < 10240000L) {
		printf("%4luK", number / 1024);
	} else if(number < 10485760000L) {
		printf("%4luM", number / (1024 * 1024));
	} else {
		printf("%4luG", number / (1024 * 1024 * 1024));
	}
}

