/****************************************************************************
 * Functions to retrieve and display process information.
 ****************************************************************************/

#include "main.h"

#define STATES 7
typedef enum {
	STATE_SLEEP,
	STATE_WAIT,
	STATE_RUN,
	STATE_IDLE,
	STATE_ZOMBIE,
	STATE_STOP,
	STATE_READY
} ProcessStateType;

typedef struct {
	pid_t pid;
	pid_t pgrp;
	char *username;
	uid_t uid;
	char command[COMMAND_SIZE + 1];
	int priority;
	unsigned long size;
	unsigned long res;
	ProcessStateType state;
	unsigned long t;
	int cpu;
	unsigned long pcpu;
	unsigned long ms;
	unsigned long threads;
} ProcessType;

typedef struct {
	pid_t pid;
	unsigned long ms;
} ProcessTimeType;

static int CompareSize(const void *a, const void *b);
static int CompareRes(const void *a, const void *b);
static int CompareCpu(const void *a, const void *b);
static int CompareTime(const void *a, const void *b);
static int CompareState(const void *a, const void *b);
static int CompareCommand(const void *a, const void *b);

static const char *STATE_NAME[] = {
	"sleep",
	"WAIT ",
	"run/%d",
	"idle ",
	"zomb ",
	"stop ",
	"ready"
};

static const char *LONG_STATE_NAME[] = {
	"sleeping",
	"waiting",
	"running",
	"idle",
	"zombie",
	"stopped",
	"ready"
};

static ProcessType *list = NULL;
static ProcessTimeType *lastList = NULL;
static unsigned long listSize = 0;
static unsigned long listMax = 0;
static unsigned long lastListSize = 0;
static unsigned long processTotal = 0;

static int state[STATES];

/****************************************************************************
 * Retrieve the process information.
 ****************************************************************************/
void GetProcessInfo() {
	struct prpsinfo info;
	DIR *procdir;
	struct dirent *entry;
	unsigned long currentLastSize;
	int fd;
	int x;

	if(!list) {
		listMax = BLOCK_SIZE;
		list = malloc(sizeof(ProcessType) * listMax);
		if(!list) {
			TerminateMessage("out of memory");
		}
	} else if(!lastList) {
		lastListSize = listSize;
		lastList = malloc(sizeof(ProcessTimeType) * lastListSize);
		if(!lastList) {
			TerminateMessage("out of memory");
		}
	} else if(lastListSize < listSize) {
		lastListSize = listSize;
		lastList = realloc(lastList, sizeof(ProcessTimeType) * lastListSize);
		if(!lastList) {
			TerminateMessage("out of memory");
		}
	}
	for(x = 0; x < listSize; x++) {
		lastList[x].pid = list[x].pid;
		lastList[x].ms = list[x].ms;
	}
	currentLastSize = listSize;

	chdir(_PATH_PROCFSPI);
	procdir = opendir(_PATH_PROCFSPI);
	if(procdir < 0) {
		TerminateMessage(strerror(errno));
	}

	listSize = 0;
	processTotal = 0;
	for(x = 0; x < STATES; x++) {
		state[x] = 0;
	}
	while((entry = readdir(procdir))) {
		fd = open(entry->d_name, O_RDONLY);
		if(fd < 0) {
			continue;
		}
		if(ioctl(fd, PIOCPSINFO, &info) < 0) {
			close(fd);
			continue;
		}
		close(fd);

		switch(info.pr_sname) {
		case 'S':
			++state[STATE_SLEEP];
			list[listSize].state = STATE_SLEEP;
			break;
		case 'R':
		case '0':
			if(info.pr_sonproc < 0) {
				++state[STATE_READY];
				list[listSize].state = STATE_READY;
			} else {
				++state[STATE_RUN];
				list[listSize].state = STATE_RUN;
			}
			break;
		case 'Z':
			++state[STATE_ZOMBIE];
			list[listSize].state = STATE_ZOMBIE;
			break;
		case 'T':
			++state[STATE_STOP];
			list[listSize].state = STATE_STOP;
			break;
		case 'I':
		case 'X':
		case 'C':
			++state[STATE_WAIT];
			list[listSize].state = STATE_WAIT;
			break;
		}

		list[listSize].cpu = info.pr_sonproc;
		list[listSize].pid = info.pr_pid;
		list[listSize].pgrp = info.pr_pgrp;
		list[listSize].priority = info.pr_pri;
		list[listSize].size = info.pr_size * pageSize;
		list[listSize].res = info.pr_rssize * pageSize;
		list[listSize].t = info.pr_time.tv_sec;
		list[listSize].threads = info.pr_thds;

		list[listSize].ms = info.pr_time.tv_sec * 100;
		list[listSize].ms += info.pr_time.tv_nsec / 10000;
		list[listSize].pcpu = 0;
		for(x = 0; x < currentLastSize; x++) {
			if(lastList[x].pid == info.pr_pid && lastList[x].ms) {
				list[listSize].pcpu = list[listSize].ms;
				list[listSize].pcpu -= lastList[x].ms;
				break;
			}
		}
		processTotal += list[listSize].pcpu;

		list[listSize].uid = info.pr_uid;
		list[listSize].username = GetUserName(info.pr_uid);
		strncpy(list[listSize].command, info.pr_fname, COMMAND_SIZE);
		list[listSize].command[COMMAND_SIZE] = 0;

		++listSize;
		if(listSize >= listMax) {
			listMax += BLOCK_SIZE;
			list = realloc(list, sizeof(ProcessType) * listMax);
			if(!list) {
				TerminateMessage("out of memory");
			}
		}
	}
	closedir(procdir);
}

/****************************************************************************
 * Print process summary.
 ****************************************************************************/
void PrintProcessInfo(int line) {
	static int putComma, x;

	printf("\033[%d;%dH\033[2K", line, 0);
	printf("%lu processes:  ", listSize);
	putComma = 0;
	for(x = 0; x < STATES; x++) {
		if(state[x]) {
			if(putComma) {
				printf(", ");
			}
			printf("%d %s", state[x], LONG_STATE_NAME[x]);
			putComma = 1;
		}
	}
}

/****************************************************************************
 * List the process information.
 ****************************************************************************/
void ListProcesses(int line, int max) {
	int x;
	int number;
	int limit;

	printf("\033[%d;%dH\033[2K\033[1m\033[7m", line, 0);
	printf("%10s %10s USERNAME PRI  SIZE   RES STATE    TIME"
		" TRS  RCPU%% COMMAND", "PID", "PGRP");
	printf("\033[0m");

	limit = (max < listSize) ? max : listSize;
	number = line + 1;
	for(x = 0; x < limit; x++) {
		if(!displayIdle && list[x].pcpu < IDLE_THRESHOLD) {
			continue;
		}
		if(displayUser >= 0 && displayUser != list[x].uid) {
			continue;
		}
		printf("\033[%d;%dH\033[2K", number, 0);
		printf("%10lu %10lu", (long)list[x].pid, (long)list[x].pgrp);
		if(list[x].username) {
			printf(" %-8s ", list[x].username);
		} else {
			printf(" %-8s ", "?");
		}
		printf("%3d ", list[x].priority);
		PrintInteger(list[x].size);
		printf(" ");
		PrintInteger(list[x].res);
		printf(" ");
		if(list[x].state == STATE_RUN) {
			printf(STATE_NAME[list[x].state], list[x].cpu);
		} else {
			printf(STATE_NAME[list[x].state]);
		}
		printf("%5lu:%02lu", list[x].t / 60, list[x].t % 60);
		printf("%4lu", list[x].threads);
		if(processTotal) {
			printf(" %6.2f ", (double)list[x].pcpu * 100.0
				/ (double)processTotal);
		} else {
			printf(" %6.2f ", 0.0);
		}
		printf("%-7s", list[x].command);
		++number;
	}
	while(number <= maxLines + line) {
		printf("\033[%d;%dH\033[2K", number, 0);
		++number;
	}

}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareSize(const void *a, const void *b) {
	return ((ProcessType*)a)->size < ((ProcessType*)b)->size;
}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareRes(const void *a, const void *b) {
	return ((ProcessType*)a)->res < ((ProcessType*)b)->res;
}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareCpu(const void *a, const void *b) {
	return ((ProcessType*)a)->pcpu < ((ProcessType*)b)->pcpu;
}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareTime(const void *a, const void *b) {
	return ((ProcessType*)a)->t < ((ProcessType*)b)->t;
}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareState(const void *a, const void *b) {
	return ((ProcessType*)a)->state < ((ProcessType*)b)->state;
}

/****************************************************************************
 * Compare function for qsort.
 ****************************************************************************/
int CompareCommand(const void *a, const void *b) {
	return strcmp(((ProcessType*)a)->command, ((ProcessType*)b)->command);
}

/****************************************************************************
 * Sort the process list.
 ****************************************************************************/
void SortProcessList() {
	int (*func)(const void*, const void*);

	unsigned long x, window;
	int swap;
	ProcessType temp;

	switch(displayOrder) {
	case ORDER_SIZE:
		func = CompareSize;
		break;
	case ORDER_RES:
		func = CompareRes;
		break;
	case ORDER_CPU:
		func = CompareCpu;
		break;
	case ORDER_TIME:
		func = CompareTime;
		break;
	case ORDER_STATE:
		func = CompareState;
		break;
	case ORDER_COMMAND:
		func = CompareCommand;
		break;
	default:
		func = CompareCpu;
		break;
	}

	/* Shell's sort */
	for(window = listSize >> 1; window; window >>= 1) {
		do {
			swap = 0;
			for(x = 0; x < listSize - window; x++) {
				if((func)(&list[x], &list[x + window]) > 0) {
					swap = 1;
					temp = list[x];
					list[x] = list[x + window];
					list[x + window] = temp;
				}
			}
		} while(swap);
	}
}

