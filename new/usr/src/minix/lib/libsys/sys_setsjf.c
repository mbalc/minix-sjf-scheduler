#include "syslib.h"

int sys_setsjf(endpoint_t proc_ep, int expected_time) { /* sjf_2018 */
        message m;

        m.m1_i1 = proc_ep;
        m.m1_i2 = expected_time;
        return(_kernel_call(SYS_SETSJF, &m));
}
