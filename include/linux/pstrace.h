#include <linux/types.h>
#include <linux/list.h>

struct pstrace {
	char comm[16];
	pid_t pid;
	long state;
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

int tracking_mode;

#define PSTRACE_BUF_SIZE 500

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
	list_add_tail(&req->list, &request_list_head);
	struct request *new_req;

	new_req = list_last_entry(&request_list_head, struct request, list);
	if(new_req == req){

		printk(KERN_INFO "[save_request] request added to linked list success");
		return 1;
	}
	else
		printk(KERN_ERR "[save_request] request not added to the list");
	return -1;
}
