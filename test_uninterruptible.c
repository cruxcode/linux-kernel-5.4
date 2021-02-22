#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/types.h>
#include <sys/wait.h>

struct pstrace {
	char comm[16];/* The name of the process*/
	pid_t pid;/* The pid of the process*/
	long state;/* The state of the process*/
};



int main(){
	int pstrace_buf_size = 500;

	FILE *fp;
	struct pstrace *buf = malloc(pstrace_buf_size*sizeof(struct pstrace));
	long *counter = malloc(sizeof(long));
	int sys_res = 0;
	*counter = 1;
	pid_t pid;
	char buffer[100];

	pid = fork();
	if (pid < 0){
		printf("FORK FAILED\n");
	}
	else if (pid == 0){
		int i = 0;
		sleep(1);
		//fp = fopen("/dev/tty0","r");
		//fread(buffer, sizeof(char), 15, fp);
		while (i < 1000){
			//fp = fopen("/dev/tty0","r");
			//printf("starting\n");
			//fread(buffer, sizeof(char), 15, fp);
			//fclose(fp);
			sleep(0.01);
			i++;
			printf("completing\n");
		}
		fp = fopen("/dev/tty0","r");
		fread(buffer, sizeof(char), 15, fp);
		fclose(fp);
		exit(0);
	}
	else{
		printf("THE PID I AM ENABLING %d\n", pid);
		sys_res = syscall(436, pid);
		sleep(10);
		if (sys_res < 0){
			printf("SOME ERROR IN ENABLE\n");
		}
		sleep(2);
		sys_res = syscall(438, pid, buf, counter);

		if (sys_res < 0){
			printf("SOME ERROR IN GET\n");
		}
		for (int j = 0; j < pstrace_buf_size; j++){
			struct pstrace *res = buf + j;
			printf("pid %u state %lu comm %s\n", res->pid, res->state, res->comm);

		}
		sys_res = syscall(437, pid);
		printf("DISABLING: %d", pid);
		if (sys_res < 0){
			printf("SOME ERROR IN DISABLE\n");
		}
		wait(NULL);
	}
	return 0;
}

