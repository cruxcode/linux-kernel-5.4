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
	int num_iterations = atoi(argv[1]);
	FILE *output_file;

	output_file = fopen("chronology_output.txt", "w");

	struct pstrace *buf = malloc(pstrace_buf_size*
			num_iterations*sizeof(struct pstrace));
	long *counter = malloc(sizeof(long));
	long counter_array[num_iterations];
	int sys_res = 0;
	*counter = 1;
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		free(buf);
		free(counter);
		fprintf(output_file, "Fork Failed\n");
		return 1;
	} else if (pid == 0) {
		while (1)
			sleep(0.01);
		exit(0);
	} else {
		fprintf(output_file,
				"THE PID I AM ENABLING %d\n", pid);
		sys_res = syscall(436, pid);
		fprintf(output_file,
				"sys_res after enable %d\n", sys_res);
		if (sys_res < 0)
			fprintf(output_file, "SOME ERROR IN ENABLE\n");
		sleep(5);
		for (int i = 0; i < num_iterations; i++) {
			//kill(pid, SIGTSTP);
			//sleep(.1);
			//kill(pid, SIGINT);
			//sleep(.1);
			//kill(pid, SIGKILL);
			//sleep(.1);
			if (i > 0)
				kill(pid, SIGCONT);
			sys_res = syscall(438, pid,
					buf + pstrace_buf_size*i, counter);
			counter_array[i] = *counter;
			if (sys_res < 0)
				fprintf(output_file, "SOME ERROR IN GET\n");
			kill(pid, SIGTSTP);
			sleep(0.1);
		}
		kill(pid, SIGINT);
		sleep(0.1);
		kill(pid, SIGKILL);
		sleep(0.1);
		sys_res = syscall(437, pid);
		fprintf(output_file, "PID I AM DISABLING %d\n", pid);
		if (sys_res < 0)
			fprintf(output_file, "SOME ERROR IN DISABLE\n");
	}
	for (int j = 0; j < pstrace_buf_size * num_iterations; j++) {
		if (j%pstrace_buf_size == 0)
			fprintf(output_file,
					"STARTING ITERATION NUMBER %d WITH COUNTER %ld\n",
					j/pstrace_buf_size,
					counter_array[j / pstrace_buf_size]);
		struct pstrace *res = buf + j;

		fprintf(output_file,
				"pid %u state %lu comm %s\n",
				res->pid, res->state, res->comm);
	}
	fclose(output_file);
	free(buf);
	free(counter);
	return 0;
}
