#include <linux/types.h>

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
};
