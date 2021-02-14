#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <linux/types.h>


//end of handlers and structures

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
