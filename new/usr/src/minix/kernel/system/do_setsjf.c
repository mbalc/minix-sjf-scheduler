#include "kernel/system.h"
#include <stdio.h>
#include <minix/endpoint.h>
#include "kernel/clock.h"

/*===========================================================================*
 *				do_setsjf  				     *
 *===========================================================================*/
int do_setsjf(struct proc * caller, message * m_ptr)
{
	printf("do_setsjf succ!\n");
	struct proc *p;
	int proc_nr;
	int expected_time;


	if (!isokendpt(m_ptr->m_lsys_krn_schedule.endpoint, &proc_nr)) {
		printf("endpoint not ok\n");
		return EINVAL;
	}

	p = proc_addr(proc_nr);
	printf("kernel call from pid=%d\n", proc_nr);

	/* lesser TODO: Process' SJF status can only be modified by the process itself */
	//if (caller != p)
	//	return(EPERM);

	/* Try to schedule the process. */
	expected_time = m_ptr->m_lsys_krn_schedule.cpu;

	// TODO: set priority and quantum basing on expected_time
	return OK;
}
