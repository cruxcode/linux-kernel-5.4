#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/types.h>


//end of handlers and structures

/*
436 - enable
437 - disable
438 - get
439 - pstrace_clear
*/

int main(int argc, char **argv)
{
	struct prinfo *buf;
	int *nr;
	pid_t pid = 0;
	long result;
	result = syscall(436, pid);
	result = syscall(437, pid);
	result = syscall(439, pid);
	result = syscall(438, pid ,buf, nr);
	result = syscall(439, pid);

	return result;
}

/*using the counter value that is returned from the sys 
call to make successive calls to prove that results 
are in a chronological order:
First pass:
1. enable all pids to be tracked.
2. get with pid = 0.
3. keep getting with subsequent counters that have been returned.
*/


