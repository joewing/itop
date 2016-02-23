/****************************************************************************
 * Functions to retrieve and display memory statistics.
 ****************************************************************************/

#include "main.h"

/****************************************************************************
 * Display memory statistics on the current line.
 ****************************************************************************/
void DisplayMemoryInfo() {
	struct rminfo64 info;
	struct sgt_cookie cookie;

	SGT_COOKIE_INIT(&cookie);
	if(sysget(SGT_RMINFO64, (char*)&info, sizeof(info), SGT_READ,
		&cookie) == -1) {
		TerminateMessage("sysget failed");
	}

	printf("Memory:");
	PrintInteger(info.physmem * pageSize);
	printf(" max,");

	PrintInteger(info.availrmem * pageSize);
	printf(" avail,");

	PrintInteger(info.freemem * pageSize);
	printf(" free,");
}


