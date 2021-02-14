
#include<linux/sched.h>
#include<linux/sched/task.h>
#include<linux/syscalls.h>
#include<linux/uaccess.h>

#define PSTRACE_BUF_SIZE 500	/* The maximum size of the ring buffer */

int process_tracking_flag = 0;  /*0 by default if process enable is being called for specific 
				processes, -1 if process enable has been called with pid = -1
				(signifying that we need to track all processes and 1 if 
				disable has been called with -1*/

int ring_buf_lock = 0;		/*If lock acquired, 1, else 0. 
				TODO: FIGURE OUT HOW TO MAKE THIS ATOMIC USING LOCK_PREFIX*/

pid_t enabled_processes[PSTRACE_BUF_SIZE]; 	/*Array that contains list of enabled processes
						This is only used when 
						process_tracking_flag != -1
						Specifically useful when process_tracking_flag = 1
						and enable call for a specific process comes.
						NOTE: Can change this datastructure later when we are optimizing*/

pid_t disabled_processes[PSTRACE_BUF_SIZE];	/*Array that contains list of disabled processes
						This is only used when process_tracking_flag != 1. 
						Specifically useful when process_tracking_flag = -1
						and disable call for a specific process comes. In that case
						we may need to provision more space if number of processes 
						to disable > PSTRACE_BUF_SIZE.
						NOTE: Can change this datastructure later when we are optimizing*/

int ring_buf_counter = 0;

/*
Temporary way of checking if a pid exists. Returns NULL if it does not.
This might be an overkill because everytime a pid exists, it will return
the whole taskstruct.
*/
static struct task_struct *get_root(int root_pid)
{
        if (root_pid == 0)
                return &init_task;

        return find_task_by_vpid(root_pid);
}

struct pstrace ring_buf[PSTRACE_BUF_SIZE];

/*TODO: We may need to implement another datastructure to put into ringbuf*/

/*
Plan of Action:
1. At the moment we will only track state changes of given pids (or all pids if -1 is sent) after the enable_function is called. This means if a process is in exit zombie when the call was made, we will not keep track of that - when it becomes exit dead, we will be able to track it. Need to check if we should track current state of a process too.
2. Currently, the plan is to call the pstrace_add from all the necessary places (such as core.c) and pstrace_add will do the check to add into ring buffer.
3. Datastructure to be used for ring buffer - hashmap or circular linked list.
4. Need to add the get_root function from hw2 to check if pid exists. Is there another way to do this?
5. We will have a global flag that contains info for whether we need to 
	a. track all processes
	b. track specific processes (default)
	c. track no process.
6. Unless we get a pstrace_enable with -1 as pid, we will track only max of 500 processes at any point in time. 
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
	bool to_track = false;
	
	if (process_tracking_flag != -1){
		int i;	
		for (i = 0; i < PSTRACE_BUF_SIZE; i++){
			if (p->pid == enabled_processes[i]){
				to_track = true;
				break;	
			}
		}
	}
	else{
		to_track = true;
	}
	if (to_track){
		struct pstrace tmp_pstrace;
		int num_chars = 0;
		while (p->comm[num_chars] != '\0'){
			tmp_pstrace.comm[num_chars] = p->comm[num_chars];
			num_chars += 1;
		}
		//tmp_pstrace.comm = p->comm;
		tmp_pstrace.pid = p->pid;
		if (p->state <= 0){
			tmp_pstrace.state = p->state;
		}
		else{
			tmp_pstrace.state = p->exit_state;		/*Will this cause problems because 
									tmp_pstrace.state is of type long
									whereas p.exit_state is an int.*/		
		}
		while(true){						/*TODO: Have a max wait time here
									for lock to be acquired.*/
			if (ring_buf_lock == 0){
				ring_buf_lock = 1;
				ring_buf[ring_buf_counter % PSTRACE_BUF_SIZE] = tmp_pstrace;
				ring_buf_counter += 1;
				ring_buf_lock = 0;
				break;
			}
		}
	}
	printk("Inside Add");
	return;
}
