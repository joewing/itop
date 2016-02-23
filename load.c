/****************************************************************************
 * Functions for retrieving and displaying the load averages.
 ****************************************************************************/

#include "main.h"

/****************************************************************************
 * Display the load averages at the current cursor position.
 ****************************************************************************/
void PrintLoadAverages() {
	struct sgt_cookie cookie;
	int load[3];

	SGT_COOKIE_INIT(&cookie);
	SGT_COOKIE_SET_KSYM(&cookie, KSYM_AVENRUN);
	if(sysget(SGT_KSYM, (char*)load, sizeof(load), SGT_READ,
		&cookie) == -1) {
		TerminateMessage("sysget failed");
	}

	printf("load averages: %.02f %.02f %.02f",
		(float)load[0] / 1024.0,
		(float)load[1] / 1024.0,
		(float)load[2] / 1024.0);
}

