#include "kernel/system.h"
#include <stdio.h>
#include <minix/endpoint.h>
#include <minix/config.h>
#include "kernel/clock.h"
#include <errno.h>

/*===========================================================================*
 *				do_setsjf  				     *
 *===========================================================================*/
int do_setsjf(struct proc * caller, message * m_ptr) { /* sjf_2018 */
	struct proc *p;
	int proc_nr, new_priority = SJF_Q,
		new_quantum = SJF_QUANTUM, cpu = -1;
	int result, expected_time = m_ptr->m1_i2;

	if (expected_time < SJF_MIN_EXP_TIME || SJF_MAX_EXP_TIME < expected_time) 
		return EINVAL;

	if (!isokendpt(m_ptr->m1_i1, &proc_nr)) {
		printf("endpoint not ok\n");
		return EINVAL;
	}

	p = proc_addr(proc_nr);

	/* lesser TODO: Only the process itself should be able to modify its SJF status - not specified in task */
	//if (caller != p)
	//	return(EPERM);

	if (expected_time == SJF_RESET_POLICY) { // restore scheduling policy to non-SJF default
		if (p->p_priority != SJF_Q) {
			return EPERM;  // process is already scheduled with default method
		}
		// TODO remember last priority and reuse it here
		if (p->p_scheduler) {  // SCHED manages this process
			new_priority = USER_Q;
		}
		else {  // no scheduler - managed by kernel
			new_priority = TASK_Q;
		}
		new_quantum = USER_QUANTUM;
	}

	else { // switch to scheduling with SJF
		if (p->p_priority == SJF_Q) {
			return EPERM;  // process already scheduled with SJF method
		}
	}

	p->expected_time = expected_time;

	result = sched_proc(p, new_priority, new_quantum, cpu);
	if (result != OK) return result;
	return -(_SIGN new_priority);  // return with sign other that the one for returning errors
}
