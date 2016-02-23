/****************************************************************************
 * Main functions.
 ****************************************************************************/

#include "main.h"

uid_t displayUser = -1;
unsigned long delay = 2;
int displayIdle = 0;
int displayLines = INT_MAX;
int maxLines = 0;
OrderType displayOrder = ORDER_CPU;

void PrintUname();
void PrintTime();
void GetLines();
void UpdateDisplay();
void AlarmHandler();
void Terminate();
void TerminateMessage(const char *msg);
void DisplaySwapInfo();

/****************************************************************************
 * Entry point.
 ****************************************************************************/
int main(int argc, char *argv[]) {
	int x;

	GetPageSize();

	for(x = 1; x < argc; x++) {
		if(argv[x][0] == '-') {
			if(argv[x][2] != 0) {
				DisplayUsage(argv[0]);
				exit(1);
			} else {
				switch(argv[x][1]) {
				case 'h':
					DisplayUsage(argv[0]);
					exit(0);
				case 'i':
				case 'I':
					displayIdle = 1;
					break;
				case 'n':
					if(x + 1 >= argc) {
						DisplayUsage(argv[0]);
						exit(1);
					} else {
						displayLines = atoi(argv[x + 1]);
						if(displayLines < 0) {
							displayLines = 0;
						}
						++x;
					}
					break;
				case 's':
					if(x + 1 >= argc) {
						DisplayUsage(argv[0]);
						exit(1);
					} else {
						delay = atoi(argv[x + 1]);
						if(delay <= 0) {
							delay = 1;
						}
						++x;
					}
					break;
				default:
					DisplayUsage(argv[0]);
					exit(1);
				}
			}
		} else {
			DisplayUsage(argv[0]);
			exit(1);
		}
	}

	printf("\033[2J");
	CommandLoop();

	return 0;
}

/****************************************************************************
 * Terminate the program normally.
 ****************************************************************************/
void Terminate() {
	TerminateMessage(NULL);
}

/****************************************************************************
 * Terminate the program with an optional message.
 ****************************************************************************/
void TerminateMessage(const char *msg) {

	system("stty sane");

	printf("\033[2J\033[0;0H");
	if(msg) {
		printf("error: %s\n", msg);
	}
	exit(0);
}

/****************************************************************************
 * Alarm handler - used for updating the display.
 ****************************************************************************/
void AlarmHandler() {
	UpdateDisplay();
	signal(SIGALRM, AlarmHandler);
	alarm(delay);
}

/****************************************************************************
 * Update the display.
 ****************************************************************************/
void UpdateDisplay() {

	GetLines();
	GetProcessInfo();
	SortProcessList();
	GetCPUStatus();

	/* First line */
	printf("\033[2K\033[1;0H");
	PrintUname();
	PrintLoadAverages();
	PrintTime();

	/* Second line */
	PrintProcessInfo(2);

	/* Third line */
	printf("\033[%d;%dH", 3, 0);
	printf("\033[2K");
	PrintCPUStatus();

	/* Fourth line */
	printf("\033[%d;%dH", 4, 0);
	printf("\033[2K");
	DisplayMemoryInfo();
	DisplaySwapInfo();
	
	/* Command line */
	printf("\033[%d;%dH", 5, 0);
	printf("\033[2K");

	if(displayLines < maxLines) {
		ListProcesses(6, displayLines);
	} else {
		ListProcesses(6, maxLines);
	}

	printf("\033[%d;%dH", 5, 0);
	fflush(stdout);
}

/****************************************************************************
 * Print information about the machine.
 ****************************************************************************/
void PrintUname() {
	struct utsname name;
	char temp[32];

	uname(&name);

	snprintf(temp, 32, "%s %s %s %s", name.sysname, name.nodename,
		name.release, name.machine);

	printf("%-31s", temp);

}

/****************************************************************************
 * Print the current time.
 ****************************************************************************/
void PrintTime() {
	time_t t;
	char *str;
	char temp[9];

	t = time(NULL);
	str = ctime(&t);

	snprintf(temp, 9, "%s", str + 11);
	printf("           %s", temp);
}

/****************************************************************************
 * Get the number of lines on the output device.
 ****************************************************************************/
void GetLines() {
	struct winsize sz;

	ioctl(1, TIOCGWINSZ, &sz);
	maxLines = sz.ws_row - 7;
	if(maxLines < 0) {
		maxLines = 0;
	}

}

