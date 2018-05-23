#include <unistd.h>
#include <stdio.h>
#include <lib.h>
#include <errno.h>

#include <minix/config.h>

#define ERROR -1
#define OK 0

int failwith(int status) {
	if (!status) return OK;
	errno = status;
	return ERROR;
}

int setsjf(int expected_time)
/* sjf_2018 */
{
	message m;
	m.m1_i1 = expected_time;

	if (expected_time < 0 || MAX_SJFPRIO < expected_time)
		return failwith(EINVAL); // argument out of range
	
	return failwith(_syscall(PM_PROC_NR, PM_SETSJF, &m));  // in case something else fails
}

