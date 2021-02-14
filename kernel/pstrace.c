
#include<linux/sched.h>
#include<linux/sched/task.h>
#include<linux/syscalls.h>
#include<linux/uaccess.h>

#define PSTRACE_BUF_SIZE 500	/* The maximum size of the ring buffer */

/*
1. At the moment we will only track state changes of given pids (or all pids if -1 is sent) after the enable_function is called. This means if a process is in exit zombie when the call was made, we will not keep track of that - when it becomes exit dead, we will be able to track it. Need to check if we should track current state of a process too.
2. Currently, the plan is to call the pstrace_add from all the necessary places (such as core.c) and pstrace_add will do the check to add into ring buffer.
3. Datastructure to be used for ring buffer - hashmap or circular linked list.
4. Need to add the get_root function from hw2 to check if pid exists. Is there another way to do this?
*/



/*
 * Syscall No. 436
 * Enable the tracing for @pid. If -1 is given, trace all processes.
 */
SYSCALL_DEFINE1(pstrace_enable,
		pid_t, pid)
{
	printk("Inside enable");
	return 0;
}

/*
 * Syscall No. 437
 * Disable the tracing for @pid. If -1 is given, stop tracing all processes.
*/
SYSCALL_DEFINE1(pstrace_disable,
		pid_t, pid)
{
	printk("Inside Disable");
	return 0;
}
/*
 * Syscall No. 438
 *
 * Copy the pstrace ring buffer info @buf.
 * If @pid == -1, copy all records; otherwise, only copy records of @pid.
 * If @counter > 0, the caller process will wait until a full buffer can
 * be returned after record @counter (i.e. return record @counter + 1 to 
 * @counter + PSTRACE_BUF_SIZE), otherwise, return immediately.
 *
 * Returns the number of records copied.
 */
SYSCALL_DEFINE3(pstrace_get,
		pid_t , pid,
		struct pstrace __user * , buf,
		int __user * , counter)
{
	printk("Inside Get");
	return 0;
}

/*
 * Syscall No.439
 *
 * Clear the pstrace buffer. If @pid == -1, clear all records in the buffer,
 * otherwise, only clear records for the give pid.  Cleared records should
 * never be returned to pstrace_get.
 */
SYSCALL_DEFINE1(pstrace_clear,
		pid_t , pid)
{
	printk("Inside Clear");
	return 0;
}

/* Add a record of the state change into the ring buffer. */
void pstrace_add(struct task_struct *p){
	
	printk("Inside Add");
	return;
}
