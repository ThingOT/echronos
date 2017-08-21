/*| provides |*/
preempt-nosched
preempt

/*| requires |*/
task

/*| doc_header |*/
/*| doc_concepts |*/
## Preemption

The RTOS is non-preemptive, which means that any context switch (see [Task Switching]) is actively caused by code that the current task executes.
In other words, once a task becomes the current task, it is in full control of the CPU, which it can relinquish only by actively causing a task switch.

This also means that the system never performs a context switch asynchronously on its own or without an explicit action from the current task itself.
To put it another way, only tasks are active entities in a system, whereas the RTOS merely provides passive library code invoked by tasks.

For example, if any task enters an infinite loop such as `for (;;) ;`, the system effectively stops.
Once inside the loop, the system no longer has the opportunity to execute any code that could lead to a task switch.
Therefore, to ensure that the system as a whole operates correctly, it is important that tasks perform context switches themselves.


/*| doc_api |*/
/*| doc_configuration |*/
/*| doc_footer |*/
