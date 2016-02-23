/****************************************************************************
 * Functions for handling user interaction.
 ****************************************************************************/

#include "main.h"

static void KillCommand();
static void ReniceCommand();
static void IdleCommand();
static void NumberCommand();
static void SecondsCommand();
static void OrderCommand();
static void UserCommand();
static void InvalidCommand();
static int GetNumber(unsigned long *value);

/****************************************************************************
 * The user interaction loop.
 ****************************************************************************/
void CommandLoop() {
	char ch;

	signal(SIGTERM, Terminate);
	signal(SIGQUIT, Terminate);
	signal(SIGKILL, Terminate);
	signal(SIGINT, Terminate);
	AlarmHandler();

	system("stty raw -echo");

	for(;;) {
		if(read(0, &ch, 1) == 1) {
			signal(SIGALRM, SIG_IGN);
			switch(ch) {
			case 'q':
				Terminate();
				break;
			case 'k':
				KillCommand();
				break;
			case 'r':
				ReniceCommand();
				break;
			case 'i':
			case 'I':
				IdleCommand();
				break;
			case 'h':
			case '?':
				DisplayHelp();
				break;
			case 'n':
			case '#':
				NumberCommand();
				break;
			case 's':
				SecondsCommand();
				break;
			case 'u':
				UserCommand();
				break;
			case 'o':
				OrderCommand();
				break;
			default:
				InvalidCommand();
				break;
			}
			fflush(stdout);
			AlarmHandler();
		}
	}
}

/****************************************************************************
 * Read a number from the user.
 ****************************************************************************/
int GetNumber(unsigned long *value) {
	int len;
	char ch;

	fflush(stdout);

	*value = 0;
	for(len = 0; len < 8;) {
		if(read(0, &ch, 1) == 1) {
			if(ch >= '0' && ch <= '9') {
				printf("%c", ch);
				*value = *value * 10 + ch - '0';
				++len;
			} else if(ch == '\n' || ch == '\r') {
				break;
			} else if(ch == '\b') {
				if(len) {
					printf("\033[1D \033[1D");
					--len;
					*value /= 10;
				}
			}
			fflush(stdout);
		}
	}
	return len;
}

/****************************************************************************
 * Read a string from the user.
 ****************************************************************************/
int GetString(char *str, int max) {
	int len;
	char ch;

	fflush(stdout);
	for(len = 0; len < max; ) {
		str[len] = 0;
		if(read(0, &ch, 1) == 1) {
			if(isgraph((int)ch)) {
				printf("%c", ch);
				str[len++] = ch;
			} else if(ch == '\n' || ch == '\r') {
				break;
			} else if(ch == '\b') {
				if(len) {
					printf("\033[1D \033[1D");
					--len;
				}
			}
			fflush(stdout);
		}
	}
	return len;
}

/****************************************************************************
 * Kill a process.
 ****************************************************************************/
void KillCommand() {
	unsigned long pid;

	printf("kill: ");
	if(GetNumber(&pid)) {
		if(kill(pid, SIGTERM)) {
			printf("\033[%d;%dH\033[2K", 5, 0);
			switch(errno) {
			case EINVAL:
				printf("invalid signal");
				break;
			case EPERM:
				printf("permission denied");
				break;
			case ESRCH:
				printf("no such PID");
				break;
			default:
				printf("process could not be killed");
				break;
			}
			fflush(stdout);
			sleep(2);
		}
	}
}

/****************************************************************************
 * Renice a process.
 ****************************************************************************/
void ReniceCommand() {
	unsigned long priority;
	unsigned long pid;

	printf("\033[2Krenice priority (0 - 40): ");
	if(GetNumber(&priority)) {
		printf("\033[%d;%dH\033[2Krenice PID: ", 5, 0);
		if(GetNumber(&pid)) {
			errno = 0;
			setpriority(PRIO_PROCESS, pid, priority - 20);
			if(errno) {
				printf("\033[%d;%dH\033[2K", 5, 0);
				switch(errno) {
				case ESRCH:
					printf("no such PID");
					break;
				case EINVAL:
					printf("invalid");
					break;
				case EPERM:
					printf("permission denied");
					break;
				case EACCES:
					printf("must be super-user");
					break;
				default:
					printf("process could not be reniced");
					break;
				}
				fflush(stdout);
				sleep(2);
			}
		}
	}
}

/****************************************************************************
 * Toggle the display of idle processes.
 ****************************************************************************/
void IdleCommand() {
	if(displayIdle) {
		displayIdle = 0;
		printf("Not displaying idle processes");
	} else {
		displayIdle = 1;
		printf("Displaying idle processes");
	}
	fflush(stdout);
	sleep(2);
}

/****************************************************************************
 * Set the number of processes to display.
 ****************************************************************************/
void NumberCommand() {
	unsigned long number;

	printf("Number of processes to show: ");
	if(GetNumber(&number)) {
		displayLines = number;
		if(displayLines < 0) {
			displayLines = 0;
		}
	}
}

/****************************************************************************
 * Set the number of seconds delay between updates.
 ****************************************************************************/
void SecondsCommand() {
	unsigned long seconds;

	printf("Seconds to delay: ");
	if(GetNumber(&seconds)) {
		delay = seconds;
		if(delay <= 0) {
			delay = 2;
		}
	}
}

/****************************************************************************
 * Select a user whose processes to display.
 ****************************************************************************/
void UserCommand() {
	uid_t temp;
	char str[9];

	printf("user (+ for all): ");
	if(GetString(str, 8)) {
		if(!strcmp(str, "+")) {
			displayUser = -1;
		} else {
			temp = GetUserID(str);
			if(temp < 0) {
				printf("\033[%d;%dH\033[2K", 5, 0);
				printf("invalid user name");
				fflush(stdout);
				sleep(2);
			} else {
				displayUser = GetUserID(str);
			}
		}
	}
}

/****************************************************************************
 * Set the sort order.
 ****************************************************************************/
void OrderCommand() {
	char str[9];

	printf("Sort order (size, res, cpu, time, state, command): ");
	if(GetString(str, 8)) {
		printf("\033[%d;%dH\033[2K", 5, 0);
		if(!strcmp(str, "size")) {
			printf("ordering by size");
			displayOrder = ORDER_SIZE;
		} else if(!strcmp(str, "res")) {
			printf("ordering by res");
			displayOrder = ORDER_RES;
		} else if(!strcmp(str, "cpu")) {
			printf("ordering by cpu");
			displayOrder = ORDER_CPU;
		} else if(!strcmp(str, "time")) {
			printf("ordering by time");
			displayOrder = ORDER_TIME;
		} else if(!strcmp(str, "state")) {
			printf("ordering by state");
			displayOrder = ORDER_STATE;
		} else if(!strcmp(str, "command")) {
			printf("ordering by command");
			displayOrder = ORDER_COMMAND;
		} else {
			printf("invalid order");
			fflush(stdout);
			sleep(2);
			return;
		}
		fflush(stdout);
		sleep(2);
	}
}

/****************************************************************************
 * Show that an invalid command was entered.
 ****************************************************************************/
void InvalidCommand() {
	printf("Invalid command");
	fflush(stdout);
	sleep(2);
}

