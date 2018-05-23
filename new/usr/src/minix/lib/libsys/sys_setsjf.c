#include "syslib.h"

int sys_setsjf(endpoint_t proc_ep, int expected_time)
{
        message m;

        m.m_lsys_krn_schedule.endpoint = proc_ep;
        m.m_lsys_krn_schedule.cpu = expected_time;
        return(_kernel_call(SYS_SETSJF, &m));
}
