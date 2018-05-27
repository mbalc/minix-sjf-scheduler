# Notice

Before building, one is required to copy headers from `/usr/src/include` to `/usr/include`
and from `/usr/src/minix/include/minix` to `/usr/include/minix`.

Changes made in this patch require remaking following directories (make && make install)

`/usr/src/minix/kernel`
`/usr/src/lib/libc`
`/usr/src/minix/lib/libsys`
`/usr/src/minix/commands/service`
`/usr/src/minix/servers/pm`
`/usr/src/minix/servers/sched`

After making, one should `make do-hdboot` in `/usr/src/releasetools` and then reboot


# System call order and specification:

- libc/misc/setsjf.c:
  - checks for argument errors
  - calls an available pm syscall and "pipes" eventual error codes
    - sets errno sign according to _SIGN (that seems to be opposite in this scope from how its
      value is defined in syscall scope
- pm/schedule.c::do_setsjf(void)
  - calls kernel call (which either fails or returns a new process priority
    - if it fails it returns an error code
  - if process has a scheduler, it calls a relevant sched call
  - returns 0 if OK or some errno.h status otherwise
- kernel/system/do_setsjf.c
  - checks if the process does or doesn't already belong to SJF
  - if the current state if correct, finds what the new priority for the state should be
  - remembers what was the last priority for this process to be able to revert when the process
    requests switching back to non-SJF
  - reschedules the process with a new priority and returns this new priority
  - a return status sign determines the type of output
    - _SIGN sign means an error
    - -(_SIGN) sign means a standard priority output
    - ...where (_SIGN) is a sign of errno.h errors as defined in default sys/errno.h header
- sched/schedule.c
  - updates its data about a given process

