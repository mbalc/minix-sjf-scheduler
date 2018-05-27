#include "pm.h"
#include <assert.h>
#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/sched.h>
#include <minix/sysinfo.h>
#include <minix/type.h>
#include <machine/archtypes.h>
#include <lib.h>
#include "mproc.h"

#include <machine/archtypes.h>
#include <minix/timers.h>
#include "kernel/proc.h"

/*===========================================================================*
 *				init_scheduling				     *
 *===========================================================================*/
void sched_init(void)
{
	struct mproc *trmp;
	endpoint_t parent_e;
	int proc_nr, s;
 
	for (proc_nr=0, trmp=mproc; proc_nr < NR_PROCS; proc_nr++, trmp++) {
		/* Don't take over system processes. When the system starts,
		 * init is blocked on RTS_NO_QUANTUM until PM assigns a 
		 * scheduler, from which other. Given that all other user
		 * processes are forked from init and system processes are 
		 * managed by RS, there should be no other process that needs 
		 * to be assigned a scheduler here */
		if (trmp->mp_flags & IN_USE && !(trmp->mp_flags & PRIV_PROC)) {
			assert(_ENDPOINT_P(trmp->mp_endpoint) == INIT_PROC_NR);
			parent_e = mproc[trmp->mp_parent].mp_endpoint;
			assert(parent_e == trmp->mp_endpoint);
			s = sched_start(SCHED_PROC_NR,	/* scheduler_e */
				trmp->mp_endpoint,	/* schedulee_e */
				parent_e,		/* parent_e */
				USER_Q, 		/* maxprio */
				USER_QUANTUM, 		/* quantum */
				-1,			/* don't change cpu */
				&trmp->mp_scheduler);	/* *newsched_e */
			if (s != OK) {
				printf("PM: SCHED denied taking over scheduling of %s: %d\n",
					trmp->mp_name, s);
			}
		}
 	}
}

/*===========================================================================*
 *				sched_start_user			     *
 *===========================================================================*/
int sched_start_user(endpoint_t ep, struct mproc *rmp)
{
	unsigned maxprio;
	endpoint_t inherit_from;
	int rv;

	/* convert nice to priority */
	if ((rv = nice_to_priority(rmp->mp_nice, &maxprio)) != OK) {
		return rv;
	}
	
	/* scheduler must know the parent, which is not the case for a child
	 * of a system process created by a regular fork; in this case the 
	 * scheduler should inherit settings from init rather than the real 
	 * parent
	 */
	if (mproc[rmp->mp_parent].mp_flags & PRIV_PROC) {
		assert(mproc[rmp->mp_parent].mp_scheduler == NONE);
		inherit_from = INIT_PROC_NR;
	} else {
		inherit_from = mproc[rmp->mp_parent].mp_endpoint;
	}
	
	/* inherit quantum */
	return sched_inherit(ep, 			/* scheduler_e */
		rmp->mp_endpoint, 			/* schedulee_e */
		inherit_from, 				/* parent_e */
		maxprio, 				/* maxprio */
		&rmp->mp_scheduler);			/* *newsched_e */
}

/*===========================================================================*
 *				sched_nice				     *
 *===========================================================================*/
int sched_nice(struct mproc *rmp, int nice)
{
	int rv;
	message m;
	unsigned maxprio;

	/* If the kernel is the scheduler, we don't allow messing with the
	 * priority. If you want to control process priority, assign the process
	 * to a user-space scheduler */
	if (rmp->mp_scheduler == KERNEL || rmp->mp_scheduler == NONE)
		return (EINVAL);

	if ((rv = nice_to_priority(nice, &maxprio)) != OK) {
		return rv;
	}

	m.m_pm_sched_scheduling_set_nice.endpoint	= rmp->mp_endpoint;
	m.m_pm_sched_scheduling_set_nice.maxprio	= maxprio;
	if ((rv = _taskcall(rmp->mp_scheduler, SCHEDULING_SET_NICE, &m))) {
		return rv;
	}

	return (OK);
}
/*===========================================================================*
 *				do_setsjf				     *
 *===========================================================================*/
int do_setsjf(void) /* sjf_2018 */
{
	// if something fails during proceeding with policy switch,
	// this will always return an error code, else the result will be equal to OK = 0

	endpoint_t proc_endpoint = mp->mp_endpoint;
	int expected_time = m_in.m1_i1;
	m_in.m1_i2 = proc_endpoint;

	if (expected_time < SJF_MIN_EXP_TIME || SJF_MAX_EXP_TIME < expected_time)
                return EINVAL;  // just a sanity check
	int result = sys_setsjf(proc_endpoint, expected_time);
	int new_queue = -(_SIGN result);  // if this is normal result, this will have a sign
		// opposite to _SIGN (see comments in kernel/system/do_setsjf.c) - now this
		// will be positive for a normal return and negative for an error

	if (new_queue < TASK_Q || NR_SCHED_QUEUES < new_queue)
		return result;  // error

	if(mp->mp_scheduler) { // we need to let scheduler know which priority the process has
		m_in.m1_i1 = new_queue;  // pass a new queue number
		// lesser TODO: get a relevant sched proc nr instead of using predefined one
		return _syscall(SCHED_PROC_NR, SCHEDULING_SETSJF, &m_in);
	}
	return 0;
}

