#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <minix/syslib.h>
#include <minix/com.h>

#define VERBOSITY 1 

int main (int argc, char ** argv) {
	long long test = 35421;
	int time = 42;
	errno = 0;
	if (argc > 1) test = atoi(argv[1]);
	if (argc > 2) time = atoi(argv[2]);
	int a = 1;
	for (int i = 1; i < 6; ++i) {
		// also worth moving a line above and substituting i->1
		printf("setsjf of %d return status %d (%d)\n", time, setsjf((i % 2) * time), errno);
		printf("%d sleeps\n", time);
		sleep(2);
		printf("start of %d\n", time);

		// heavy computations w/ logging of progress
		for (int rep = 1; rep <= VERBOSITY; ++rep) {
			for (long long j = 0; j < test/VERBOSITY; ++j) {
				for (long long k = 0; k < test; ++k) {
					a *= 31;
				}
			}
			printf("%d percent of %d's job, SJF:%d\n", 100 * rep / VERBOSITY, time, i % 2);
		}
	}

	printf("finito %d\n", time);
	return 0;
}
