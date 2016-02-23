/****************************************************************************
 * Main header file.
 ****************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/sysget.h>
#include <sys/utsname.h>
#include <sys/proc.h>
#include <sys/procfs.h>
#include <sys/swap.h>
#include <sys/sysmp.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <paths.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <signal.h>

#define BLOCK_SIZE 64
#define USERNAME_SIZE 8
#define COMMAND_SIZE 7
#define IDLE_THRESHOLD 10

typedef enum {
	ORDER_SIZE,
	ORDER_RES,
	ORDER_CPU,
	ORDER_TIME,
	ORDER_STATE,
	ORDER_COMMAND
} OrderType;

#include "process.h"
#include "username.h"
#include "misc.h"
#include "command.h"
#include "help.h"
#include "cpu.h"
#include "memory.h"
#include "swap.h"
#include "load.h"

void Terminate();
void TerminateMessage(const char *msg);
void AlarmHandler();

void DisplayHelp();

extern uid_t displayUser;
extern int displayIdle;
extern int displayLines;
extern int maxLines;
extern OrderType displayOrder;
extern unsigned long delay;

#endif

