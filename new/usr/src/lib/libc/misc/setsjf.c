#include <unistd.h>
#include <stdio.h>
#include <lib.h>

#include <minix/config.h>

#include <errno.h>

#define S_ERROR -1
#define S_OK 0

int failwith(int status) { /* sjf_2018 */
	if (status == 0) return S_OK;
	if (status < 0) status = -status; // absolute value
	errno = (_SIGN status);
	return S_ERROR;
}

int setsjf(int expected_time) { /* sjf_2018 */
	message m;
	m.m1_i1 = expected_time;

	if (expected_time < SJF_MIN_EXP_TIME || SJF_MAX_EXP_TIME < expected_time) {
		return failwith(EINVAL); // argument out of range
	}
	
	return failwith(_syscall(PM_PROC_NR, PM_SETSJF, &m));  // in case something else fails
}

