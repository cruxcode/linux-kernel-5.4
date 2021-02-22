#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
};

int main(int argc, char **argv)
{
	int pstrace_buf_size = 500;
	int num_processes = 2;

	struct pstrace *buf = malloc(pstrace_buf_size*num_processes*sizeof(struct pstrace));
	long *counter = malloc(sizeof(long));
	int sys_res = 0;
	*counter = 1;
	pid_t pid;
	for (int i = 0; i < num_processes; i++){	
		pid = fork();
		if (pid < 0){
			printf("Fork Failed\n");
			return 1;
		}
		else if (pid == 0) {
			while(1){
				sleep(0.01);
			}
			exit(0);
		}
		else{
			printf("THE PID I AM ENABLING %d\n", pid);
			sys_res = syscall(436, pid);
			printf("sys_res after enable %d\n", sys_res);
			if (sys_res < 0){
				printf("SOME ERROR IN ENABLE\n");
			}
			sleep(5);
			kill(pid, SIGTSTP);
			sleep(.1);
			kill(pid, SIGINT);
			sleep(.1);
			kill(pid, SIGKILL);
			sleep(.1);
			sys_res = syscall(438, pid, buf + pstrace_buf_size*i , counter);
			if (sys_res < 0){
				printf("SOME ERROR IN GET\n");
			}	
			sys_res = syscall(437, pid);
			printf("PID I AM DISABLING %d\n", pid);
			if (sys_res < 0){
				printf("SOME ERROR IN DISABLE\n");
			}
		}	
	}
	for (int j = 0; j < pstrace_buf_size * num_processes ; j++){
		struct pstrace *res = buf + j;
		printf("pid %u state %lu comm %s\n", res->pid, res->state, res->comm);
	}
	return 0;
}
