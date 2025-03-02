#include <linux/module.h>
#include <linux/kernel.h>   
#include <linux/init.h> 
#include <linux/proc_fs.h>
#include <linux/uaccess.h>  
#include <linux/sched.h>     
#include <linux/pid.h>       
#include <linux/slab.h>      

#define PROC_NAME "pid"  
#define BUFFER_SIZE 100  

static long stored_pid = -1;
static struct proc_dir_entry *proc_entry; 

ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos);

// **Write function**
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *kmem;
    kmem = kmalloc(count + 1, GFP_KERNEL);
    if (!kmem)
        return -ENOMEM;
    
    if (copy_from_user(kmem, usr_buf, count)) {
        kfree(kmem);
        return -EFAULT;
    }

    kmem[count] = '\0';

    if (kstrtol(kmem, 10, &stored_pid)) {
        kfree(kmem);
        return -EINVAL; 
    }

    kfree(kmem);  
    printk(KERN_INFO "Stored PID: %ld\n", stored_pid);
    return count;
}

// **Read function**
ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos) {
    char data[BUFFER_SIZE];
    int len;
    struct pid *pid_struct;
    struct task_struct *task;

    if (*pos > 0) 
        return 0;  

    if (stored_pid == -1)
        return sprintf(data, "Please enter a PID first.\n");

    pid_struct = find_vpid(stored_pid);
    if (!pid_struct)
        return sprintf(data, "PID %ld not found.\n", stored_pid);

    task = pid_task(pid_struct, PIDTYPE_PID);
    if (!task)
        return sprintf(data, "No task found for PID %ld.\n", stored_pid);

    // ✅ FIXED: Use task_state_to_char instead of task->state
    char state = task_state_to_char(task);

    len = snprintf(data, sizeof(data), "Process: %s, PID: %d, State: %c\n",
                   task->comm, task->pid, state);

    if (copy_to_user(buffer, data, len))
        return -EFAULT;

    *pos = len;
    return len;
}

// **File Operations**
static struct proc_ops proc_fops= {
    .proc_read  = proc_read,
    .proc_write = proc_write,
};

// **Module Init**
static int __init pid_module_init(void) {
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry)
        return -ENOMEM;
    
    printk(KERN_INFO "/proc/%s created.\n", PROC_NAME);
    return 0;
}

// **Module Exit**
static void __exit pid_module_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed.\n", PROC_NAME);
}

module_init(pid_module_init);
module_exit(pid_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mai Elkeshen");
MODULE_DESCRIPTION("Kernel module for task (process) information.");

