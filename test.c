#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

//end of handlers and structures

/*
436 - enable
437 - disable
438 - get
439 - pstrace_clear
*/

/* The data structure used to save the traced process. */
struct pstrace {
        char comm[16];          /* The name of the process */
        pid_t pid;              /* The pid of the process */
        long state;             /* The state of the process */
};

int main()
{
        pid_t pid = -1;
        struct pstrace *buf = malloc(500*sizeof(struct pstrace));
        long counter = 0;
	syscall(436, pid);
	syscall(438, pid, buf, &counter);
        printf("syscall result %ld\n", counter);
	syscall(438, pid, buf, &counter);
        printf("syscall result %ld\n", counter);
	syscall(439, pid);
	printf("Clear done\n");
}

/*using the counter value that is returned from the sys 
call to make successive calls to prove that results 
are in a chronological order:
First pass:
1. enable all pids to be tracked.
2. get with pid = 0.
3. keep getting with subsequent counters that have been returned.
*/


