# OS - Spring 2021 - Assignment 3 - Programming Part
## George Cherian, Shyam Swaroop and Vishwesh Kirthivasan

This assignment was aimed at enabling tracking of 6 states, namely:

1. TASK\_RUNNING
2. TASK\_INTERRUPTIBLE
3. TASK\_UNINTERRUPTIBLE
4. \_\_TASK\_STOPPED
5. EXIT\_ZOMBIE
6. EXIT\_DEAD

We have implemented this by using the concept of wait\_queue coupled with kthreads. We have also used spin locks for operations on certain datastructures from within critical section only. The main kernel code is written in kernel/pstrace.c. The system calls that are available for user code to call are:

1. pstrace\_get
2. pstrace\_enable
3. pstrace\_disable
4. pstrace\_clear

Each time a user makes a pstrace_get system call, the pstrace_get in kernel creates a new kthread whose job is to keep checking if the condition has been satisfied for the pstrace_get call to finish. In pstrace_add, we add the event into the ring buffer and check if any user request has met the required condition. If yes, we copy all the data in a request datastrcutre (it holds the details of client's request like pid, counter, buffer etc.). This way we ensure that user gets the instantaneuos snapshot of the ring buffer as soon as it's condition has been satisfied. Note, we **do not** call wake_up inside pstrace_add, hence, **no deadlock occurs**.

Finally, in the "test" branch, are two files that can be used to test two very specific functionalities of the system calls. 

1. `make` followed by ```./test_chronology <num_iterations>``` will run the pstrace_enable, pstrace\_get and ps\_trace disable for processes spwaned to track the statuses TASK_INTERRUPTIBLE, TASK\_RUNNING and TASK\_UNINTERRUPTIBLE. More importantly, from the second iteration onwards, the first state that the process goes through is "TASK\_UNINTERRUPTIBLE" and this can be seen in the logs that are printed in chronology.txt. As per code, it was expected for the TASK_STOPPED to be close to top in second iteration. One possibilty of getting records with missing event is when PSTRACE_BUF_SIZE events occure between two subsequesnt pstrace_get call.

2. `make` followed by ```./six_states``` will run the pstrace\_enable, pstrace\_get and pstrace\_disable for processes spawned to track all 6 statuses mentioned above. Each of the process goes through several TASK\_RUNNING and TASK\_INTERRUPTIBLE states, with 1 each of TASK\_UNINTERRUPTIBLE, \_\_TASK\_STOPPED, EXIT\_ZOMBIE and EXIT\_DEAD. This can be seen in the logs printed in six\_states.txt.
