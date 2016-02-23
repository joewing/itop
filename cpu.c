/****************************************************************************
 * Functions for handling the CPU status.
 ****************************************************************************/

#include "main.h"

typedef struct {
	unsigned long cpu[CPU_STATES];
	unsigned long total;
} CPUStatus;

static CPUStatus cpuStatus = { { 0 } };

/****************************************************************************
 * Retrieve the CPU Status.
 ****************************************************************************/
void GetCPUStatus() {
	static CPUStatus last = { { 0 } };
	struct sysinfo current;
	struct sgt_cookie cookie;
	long total;
	int x;

	SGT_COOKIE_INIT(&cookie);
	if(sysget(SGT_SINFO, (char*)&current, sizeof(current), SGT_READ,
		&cookie) == -1) {
		TerminateMessage("sysget failed");
	}

	total = 0;
	for(x = 0; x < CPU_STATES; x++) {
		total += current.cpu[x];
	}

	for(x = 0; x < CPU_STATES; x++) {
		cpuStatus.cpu[x] = current.cpu[x] - last.cpu[x];
		last.cpu[x] = current.cpu[x];
	}
	cpuStatus.total = total - last.total;
	last.total = total;
}

/****************************************************************************
 * Display the CPU status on the current line.
 ****************************************************************************/
void PrintCPUStatus() {
	printf("CPU:%5.01f%% idle,%5.01f%% usr,%5.01f%% ker,"
		"%5.01f%% wait,%5.01f%% xbrk,%5.01f%% intr\n",
		(double)cpuStatus.cpu[CPU_IDLE] / cpuStatus.total * 100.0,
		(double)cpuStatus.cpu[CPU_USER] / cpuStatus.total * 100.0,
		(double)cpuStatus.cpu[CPU_KERNEL] / cpuStatus.total * 100.0,
		(double)cpuStatus.cpu[CPU_WAIT] / cpuStatus.total * 100.0,
		(double)cpuStatus.cpu[CPU_SXBRK] / cpuStatus.total * 100.0,
		(double)cpuStatus.cpu[CPU_INTR] / cpuStatus.total * 100.0);
}

