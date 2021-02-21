#include<linux/sched.h>
#include<linux/sched/task.h>
#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/pstrace.h>

/*
 * Syscall No. 436
 * Enable the tracing for @pid. If -1 is given, trace all processes.
 */
SYSCALL_DEFINE1(pstrace_enable,
		pid_t, pid)
{
	printk("[pstrace_enable]");
	return 0;
}

/*
 * Syscall No. 437
 * Disable the tracing for @pid. If -1 is given, stop tracing all processes.
*/
SYSCALL_DEFINE1(pstrace_disable,
		pid_t, pid)
{
	printk("[pstrace_disable]");
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
		long __user * , counter)
{
	printk("[pstrace_get]");
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
	printk("[pstrace_clear]");
	return 0;
}

/* Add a record of the state change into the ring buffer. */
void pstrace_add(struct task_struct *p){
	printk("[pstrace_add]");	
}
