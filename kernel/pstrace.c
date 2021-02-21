#include<linux/syscalls.h>
#include<linux/uaccess.h>
#include<linux/pstrace.h>
#include <linux/string.h>
#include <linux/slab.h>
/*
 * Syscall No. 436
 * Enable the tracing for @pid. If -1 is given, trace all processes.
 */
SYSCALL_DEFINE1(pstrace_enable,
		pid_t, pid)
{
	printk("[pstrace_enable]");
	if(pid == -1)
		tracking_mode = TRACK_ALL;
	printk("[pstrace_enable] tracking mode set to %d", tracking_mode);
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
	if(pid == -1)
		tracking_mode = TRACK_NONE;
	printk("[pstrace_disable] tracking mode set to %d", tracking_mode);
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
	int success;

	if(tracking_mode == TRACK_ALL){
		long *kcounter;
		struct pstrace *kbuf;

		kcounter = kmalloc(sizeof(long), GFP_KERNEL);
		if(!kcounter)
			return -ENOMEM;
		success = copy_from_user(kcounter, counter, sizeof(long));
		if(success != 0){
			kfree(kcounter);
			return -EFAULT;
		}

		kbuf = kmalloc(sizeof(struct pstrace)*PSTRACE_BUF_SIZE, GFP_KERNEL);
		if (!kbuf) {
			kfree(kcounter);
			return -ENOMEM;
		}
		
		struct request *req;
		
		req = kmalloc(sizeof(struct request), GFP_KERNEL);

		if(!req){
			kfree(kbuf);
			kfree(kcounter);
			return -ENOMEM;
		}

		unsigned long flags;
		create_request(req, pid, kcounter, kbuf);
		spin_lock_irqsave(&request_list_lock, flags);
		success = save_request(req);
		spin_unlock_irqrestore(&request_list_lock, flags);
		if(success < 0){
			kfree(req);
			kfree(kbuf);
			kfree(kcounter);
			return -ENOMEM;
		}
		
		struct task_struct *handler;

		handler = listener(req);

		if(!handler){
			printk("[pstrace_get] handler null before while starts");
		}

		DEFINE_WAIT(wait);
		while(!(req->complete_flag)){
			printk("[pstrace_enable] calling prepare_to_sleep");
			prepare_to_wait(&pstrace_wait_q, &wait, TASK_INTERRUPTIBLE);
			if(signal_pending(current)){
				printk("[pstrace_get] signal is pending");
				if(!handler)
					printk(KERN_WARNING "[pstrace_get] handler is null");
				else
					thread_cleanup(handler);
				break;
			}
			schedule();
			printk("[pstrace_enable] woke up from sleep");
		}
		printk("[pstrace_enable] calling finish_wait");
		finish_wait(&pstrace_wait_q, &wait);
		printk("[pstrace_enable] finsihed waiting");
		success = copy_to_user(buf, kbuf, sizeof(struct pstrace) * PSTRACE_BUF_SIZE);
		if (success != 0){
			kfree(req);
			kfree(kbuf);
			kfree(kcounter);
			return -EFAULT;
		}
		success = copy_to_user(counter, kcounter, sizeof(long));
		if (success != 0){
			kfree(req);
			kfree(kbuf);
			kfree(kcounter);
			return -EFAULT;
		} 
		kfree(req);
		kfree(kbuf);
		kfree(kcounter);
	}
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
	unsigned long flags;
	unsigned long ring_buf_flags;
	unsigned long request_list_flags;
	if(p->state == TASK_STOPPED || p->state == TASK_INTERRUPTIBLE || p->state == TASK_UNINTERRUPTIBLE|| p->state == TASK_RUNNING){
		//printk("[pstrace_add]");
		if (tracking_mode == TRACK_ALL){
			local_irq_save(flags);
			spin_lock_irqsave(&request_list_lock, request_list_flags);
			spin_lock_irqsave(&ring_buf_lock, ring_buf_flags);
			memcpy(ring_buffer.buf[ring_buffer.head].comm, p->comm, sizeof(char)*16);
			ring_buffer.buf[ring_buffer.head].pid = p->pid;
			ring_buffer.buf[ring_buffer.head].state = p->state;
			ring_buffer.head = (ring_buffer.head + 1) % PSTRACE_BUF_SIZE;
			ring_buffer.counter += 1;
			ring_buffer.current_size += 1;
			if (ring_buffer.current_size > PSTRACE_BUF_SIZE){
				ring_buffer.current_size = PSTRACE_BUF_SIZE;
			}
			struct request *pos, *next;			
			list_for_each_entry_safe(pos, next, &request_list_head, list){						
				if (ring_buffer.counter == PSTRACE_BUF_SIZE + *(pos->counter)){		
					memcpy(pos->buf, ring_buffer.buf, sizeof(struct pstrace) * PSTRACE_BUF_SIZE);
					*(pos->counter) = ring_buffer.counter;
					pos->complete_flag = true;
					list_del(&pos->list);
				}
			}
			spin_unlock_irqrestore(&ring_buf_lock, ring_buf_flags);
			spin_unlock_irqrestore(&request_list_lock, request_list_flags);
			local_irq_restore(flags);
		}				
	}
}
