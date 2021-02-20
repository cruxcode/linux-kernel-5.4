#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/types.h>
struct pstrace {
	char comm[16];/* The name of the process*/
	pid_t pid;/* The pid of the process*/
	long state;/* The state of the process*/
};


//end of handlers and structures

int main(int argc, char *argv[])
{
	struct pstrace *buf = malloc(500*sizeof(struct pstrace));
	long *nr = malloc(sizeof(long));
	*nr = -1;
	char *tmp_char = argv[1];
	pid_t pid;
	if (tmp_char[0] == '-'){
		pid = atoi(&tmp_char[1]);
		pid = pid * (-1);
	}
	else{
		pid = atoi(tmp_char);
	}
	printf("PID IS %d\n", pid);
	long result = 0;
	printf("before enable\n");
	result = syscall(436, pid);
	//result = syscall(437, pid);
	//result = syscall(439, pid);
	printf("after enable before get\n");
	result = syscall(438, pid ,buf, nr);
	printf("after get\n");
	printf(" %ld \n",*nr);
	for(int i=0;i<500;i++){
		printf(" buffer state of %d is %ld and comm is %s and pid is %d\n",i,buf[i].state, buf[i].comm, buf[i].pid);
	}

	/*printf("before disable after get\n");
	result = syscall(437, -1 );
	printf("after disable\n");
	printf(" %ld \n",*nr);
	for(int i=0;i<5;i++){
		printf(" buffer state of %d is %ld\n",i,buf[i].state);
		printf(" buffer state of %d is %s\n",i,buf[i].comm);
	}
	//result = syscall(439, pid);*/

	return result;
}
