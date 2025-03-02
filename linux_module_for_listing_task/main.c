#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>  
#include <linux/list.h>

#define PROCESS_INFO "pidinfo"

// Function prototypes
void BFSIteration(void);
void linearIteration(void);

// BFS Iteration - Iterates over children of init_task
void BFSIteration(void) {
    struct task_struct *task;
    struct list_head *list;

    list_for_each(list, &init_task.children) { 
        task = list_entry(list, struct task_struct, sibling);
        printk(KERN_INFO "Name: %s | PID: %d | State Index: %u\n",
               task->comm, task->pid, task_state_index(task));
    }
}

// Linear Iteration - Iterates over all tasks
void linearIteration(void) {
    struct task_struct *task;
    for_each_process(task) {
        printk(KERN_INFO "Name: %s | PID: %d | State Index: %u\n",
               task->comm, task->pid, task_state_index(task));
    }
}

// Module init function
static int __init task_lister_init(void) {
    printk(KERN_INFO "Loading Task Lister Module...\n");
    linearIteration();
    //BFSIteration();
    return 0;
}

// Module exit function
static void __exit task_lister_exit(void) {
    printk(KERN_INFO "Exiting Task Lister Module...\n");
}

module_init(task_lister_init);
module_exit(task_lister_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mai Elkheshen");
MODULE_DESCRIPTION("Iterating over tasks using BFS and linear traversal");

