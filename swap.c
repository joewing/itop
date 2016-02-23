/****************************************************************************
 * Functions to retrieve and display swap usage.
 ****************************************************************************/

#include "main.h"

/****************************************************************************
 * Display swap usage.
 ****************************************************************************/
void DisplaySwapInfo() {
	off_t totalSwap;
	off_t freeSwap;

	swapctl(SC_GETSWAPMAX, &totalSwap);
	swapctl(SC_GETFREESWAP, &freeSwap);

	PrintInteger(totalSwap / 2);
	printf(" swap,");

	PrintInteger(freeSwap / 2);
	printf(" free swap");

}

