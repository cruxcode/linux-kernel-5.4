#include <linux/types.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include<linux/sched.h>
#include<linux/sched/task.h>

#define PSTRACE_BUF_SIZE 500

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
};

struct pstrace_buf{
	struct pstrace buf[PSTRACE_BUF_SIZE];
	long head;
	long current_size;	
	long counter;
};

struct request {
	pid_t pid;
	struct pstrace *buf;
	long *counter;
	bool complete_flag;
	struct list_head list;
};

#define TRACK_ALL 1
#define TRACK_NONE 2
#define TRACK_ALL_EXCEPT 3
#define TRACK_SOME 4

//Process lists and spinlocks for processes
pid_t enabled_processes[PSTRACE_BUF_SIZE];
pid_t disabled_processes[PSTRACE_BUF_SIZE];
int enabled_process_count = 0;
int disabled_process_count = 0;
DEFINE_SPINLOCK(process_list_lock);

struct pstrace_buf ring_buffer;
DEFINE_SPINLOCK(ring_buf_lock);

int tracking_mode;

wait_queue_head_t pstrace_wait_q;
DECLARE_WAIT_QUEUE_HEAD(pstrace_wait_q);

LIST_HEAD(request_list_head);
DEFINE_SPINLOCK(request_list_lock);

void create_request(struct request * req, pid_t pid, long *counter, struct pstrace *buf)
{
	req->pid = pid;
	req->counter = counter;
	req->buf = buf;
	req->complete_flag = false;
}

int save_request(struct request *req)
{
	struct request *new_req;
	list_add_tail(&req->list, &request_list_head);

	new_req = list_last_entry(&request_list_head, struct request, list);
	if(new_req == req){

		printk(KERN_INFO "[save_request] request added to linked list success");
		return 1;
	}
	else
		printk(KERN_ERR "[save_request] request not added to the list");
	return -1;
}

int listener_fn(void *data)
{
	struct request *new_data;
	new_data = (struct request *)data;
	printk("[listener_fn] called");
	while(true){
		if(kthread_should_stop()){
			break;
		}
		if (new_data->complete_flag){
			wake_up(&pstrace_wait_q);
			break;
		}
		schedule();
	}
	printk("[listener_fn] exiting");
	return 0;
}

struct task_struct *listener(struct request *data){
	struct task_struct *p;
	char name[] = "pstrace_thread";
	printk("[listener] called");
	p = kthread_run(listener_fn, data, name);
	if(!p){
		printk("[listener] task_struct failed to create, null pointer");	
	}
	printk("[listener] exiting listener");
	return p;
}

void thread_cleanup(struct task_struct *p)
{
	int failed;
	printk("[thread_cleanup] called");
	if(!p)
		printk("[thread_cleanup] p is null");
	failed = kthread_stop(p);
	if(!failed)
		printk("[thread_cleanup] thread stopped success");
}


int check_if_process_in_list(pid_t * processes_list,pid_t curr_pid,int count){
	int i;
	for(i = 0; i < count ; ++i)
  		if(curr_pid == processes_list[i] && curr_pid != -1)
			return i;
	return -1;
}


int reset_enabled_and_disabled(void){
	int i,j;
	for(i = 0; i < enabled_process_count; i++)
  		enabled_processes[i] = -1;
	for(j = 0; j < disabled_process_count ; j++)
  		disabled_processes[j] = -1;
	enabled_process_count = 0;
	disabled_process_count = 0;
	return 0;
}


void copy_from_buf_to_req(struct pstrace_buf *buf, struct request *req)
{
	printk("[copy_from_buf_to_req] called");
	if(buf->current_size < PSTRACE_BUF_SIZE){
		long j = 0, i;
		for(i = 0; i < buf->current_size; i++){
			struct pstrace *curr = buf->buf + i;
			if(curr->pid != -1 && (req->pid ==-1 || req->pid==curr->pid)){
				*(req->buf + j) = *curr;
				j++;
			}
		}
	} else {
		long j = 0, i;
		for(i = buf->head; i < PSTRACE_BUF_SIZE; i++){
			struct pstrace *curr = buf->buf + i;
			if(curr->pid != -1 && (req->pid ==-1 || req->pid==curr->pid)){
				*(req->buf + j) = *curr;
				j++;
			}
		}
		for(i = 0; i < buf->head; i++){
			struct pstrace *curr = buf->buf + i;
			if(curr->pid != -1 && (req->pid ==-1 || req->pid==curr->pid)){
				*(req->buf + j) = *curr;
				j++;
			}
		}
	}
	*(req->counter) = buf->counter;
}


static struct task_struct *get_root(int root_pid)
{
        if (root_pid == 0)
                return &init_task;

        return find_task_by_vpid(root_pid);
}

int is_valid_pid(pid_t pid){

	struct task_struct *root_task;
	root_task = get_root(pid);
	if(!root_task){
		return 0;
	}
	return 1;
}

