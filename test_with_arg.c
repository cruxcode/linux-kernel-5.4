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
	*nr = 1;
	FILE *output_file;

	output_file = fopen("output_for_pid.txt", "w");
	char *tmp_char = argv[1];
	pid_t pid;

	if (tmp_char[0] == '-') {
		pid = atoi(&tmp_char[1]);
		pid = pid * (-1);
	} else {
		pid = atoi(tmp_char);
	}
	fprintf(output_file, "PID IS %d\n", pid);
	long result = 0;

	result = syscall(436, pid);
	result = syscall(438, pid, buf, nr);
	fprintf(output_file, " %ld\n", *nr);
	result = syscall(438, pid, buf, nr);
	fprintf(output_file, "%ld\n", *nr);
	int num_subs_calls = 0;

	while (num_subs_calls < 1) {
		result = syscall(438, pid, buf, nr);
		for (int i = 0; i < 50; i++)
			fprintf(output_file,
			" buffer state of %d is %ld and comm is %s and pid is %d\n",
			i, buf[i].state, buf[i].comm, buf[i].pid);
		num_subs_calls += 1;
	}
	fprintf(output_file, "NR %ld", *nr);
	fclose(output_file);
	free(nr);
	free(buf);
	return result;
}
