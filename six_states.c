#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
};

int main(int argc, char **argv)
{
	int pstrace_buf_size = 500;
	int num_processes = 2;

	FILE *fp, *outfp;

	struct pstrace *buf = malloc(pstrace_buf_size*num_processes*sizeof(struct pstrace));
	long *counter = malloc(sizeof(long));
	int sys_res = 0;
	*counter = 1;
	pid_t enabled[num_processes];
	pid_t pid;
	for (int i = 0; i < num_processes; i++){	
		pid = fork();
		if (pid < 0){
			free(buf);
			free(counter);
			printf("Fork Failed\n");
			return 1;
		}
		else if (pid == 0) {
			int j = 0;
			signal(SIGTSTP, SIG_DFL);
			while(j < 500*(i + 1)){
				sleep(0.01);
				j++;
			}
			if ((i % 2) == 0){
				raise(SIGTSTP);
				fp = fopen("/dev/tty0","w");
				fprintf(fp, "Hello from curxcode");
				fclose(fp);
				exit(0);
			}
		}
		else{
			enabled[i] = pid;	
			sys_res = syscall(436, pid);
			if(sys_res != 0){
				free(buf);
				free(counter);
				exit(-1);
			}
			sleep(5);
			kill(pid, SIGCONT);
			sleep(1);
			kill(pid, SIGTSTP);
			sleep(1);
			kill(pid, SIGINT);
			sleep(0.1);
			kill(pid, SIGKILL);
			sleep(0.1);
			wait(&sys_res);
			signal(SIGCHLD, SIG_IGN);
			sys_res = syscall(438, pid, buf + pstrace_buf_size*i , counter);
			if (sys_res < 0){
				free(counter);
				free(buf);
				exit(-1);
			}	
			sys_res = syscall(437, pid);
			if (sys_res < 0){
				free(counter);
				free(buf);
				exit(-1);
			}
		}	
	}
	
	outfp = fopen("pstrace_output.txt", "w");	
	for (int j = 0; j < pstrace_buf_size * num_processes ; j++){
		if(j % pstrace_buf_size == 0)
			fprintf(outfp, "pid enabled is %d\n", enabled[j%pstrace_buf_size]);
		struct pstrace *res = buf + j;	
		fprintf(outfp, "pid %u state %lu comm %s\n", res->pid, res->state, res->comm);
	}
	fclose(outfp);
	free(counter);
	free(buf);
	return 0;
}
