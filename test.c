#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/types.h>

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
};

int main(int argc, char **argv)
{
	struct pstrace *buf = malloc(5*sizeof(struct pstrace));
	long *counter = malloc(sizeof(long));
	*counter = 100000;
	syscall(436, -1);
	printf("after enable before get\n");
	for(int j = 0; j < 2; j++){
		syscall(438, -1 ,buf, counter);
		for(int i=0; i<5; i++){
			struct pstrace *res = buf + i;
			printf("pid %u state %lu comm %s\n", res->pid, res->state, res->comm);  
		}
	}
	syscall(437, -1);
	return 0;
}
