/****************************************************************************
 * Help functions.
 ****************************************************************************/

#include "main.h"

/****************************************************************************
 * Display program usage.
 ****************************************************************************/
void DisplayUsage(const char *name) {
	printf("usage: %s [<options>]\n", name);
	printf("  options:\n");
	printf("    -h            Display this message\n");
	printf("    -i or -I      Display idle processes\n");
	printf("    -n <number>   Display <number> processes\n");
	printf("    -s <number>   Wait <number> seconds between updates\n");
}

/****************************************************************************
 * Display help.
 ****************************************************************************/
void DisplayHelp() {
	char ch;

	printf("\033[2J\033[0;0H");
	printf("jtop v1.0 by Joe Wingbermuehle");

	printf("\033[%d;%dH", 2, 0);
	printf("Commands:");

	printf("\033[%d;%dH", 4, 4);
	printf("q        - quit");
	printf("\033[%d;%dH", 5, 4);
	printf("h or ?   - display this help message");
	printf("\033[%d;%dH", 6, 4);
	printf("k        - kill a process");
	printf("\033[%d;%dH", 7, 4);
	printf("n or #   - change the number of processes to display");
	printf("\033[%d;%dH", 8, 4);
	printf("s        - change the number of seconds between updates");
	printf("\033[%d;%dH", 9, 4);
	printf("i or I   - toggle the display of idle processes");
	printf("\033[%d;%dH", 10, 4);
	printf("r        - renice a process");
	printf("\033[%d;%dH", 11, 4);
	printf("u        - show a user's processes (+ for all)");

	printf("\033[%d;%dH", 20, 0);
	printf("Press any key...");
	fflush(stdout);

	while(read(0, &ch, 1) != 1);
}
