#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/kallsyms.h>
#include <linux/types.h>
#include <linux/list.h>
#include <asm/syscall_wrapper.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/init_task.h>
#include <linux/fs.h>

#define __NR_ftrace 336

asmlinkage int (*real_ftrace)(const struct pt_regs *regs); 

void **syscall_table;       //System Call Table

static asmlinkage int process_tracer(const struct pt_regs *regs){
    int count = 0;

    pid_t pid = (pid_t)(regs->di);      //Get pid from real_ftrace

    //find task struct using pid
    struct task_struct *findtask = pid_task(find_vpid(pid), PIDTYPE_PID);
    struct task_struct *task;

    //can't find pid's task struct case
    if(findtask == NULL)    return -1;

    //print task struct's info
    printk(KERN_INFO "##### TASK INFORMATION of '' [%d] %s'' #####\n", findtask->pid, findtask->comm);
    
    //state print
    if(findtask->state == TASK_RUNNING)                 printk(KERN_INFO "-task state : Running or ready\n");
    else if(findtask->state == TASK_INTERRUPTIBLE)      printk(KERN_INFO "-task state : Wait\n");
    else if(findtask->state == TASK_UNINTERRUPTIBLE)    printk(KERN_INFO "-task state : Wait with ignoring all signals\n");
    else if(findtask->state == __TASK_STOPPED)          printk(KERN_INFO "-task state : Stopped\n");
    else if(findtask->state == EXIT_DEAD)               printk(KERN_INFO "-task state : Dead\n");
    else if(findtask->state == EXIT_ZOMBIE)             printk(KERN_INFO "-task state : Zombie process\n");
    else                                                printk(KERN_INFO "-task state : etc.\n");

    //Group Leader & context switch & fork count & parent process print
    printk(KERN_INFO "- Process Group Leader : [%d] %s\n", findtask->group_leader->pid, findtask->group_leader->comm);
    printk(KERN_INFO "- Number of context switches : %lu\n", findtask->nivcsw);
    printk(KERN_INFO "- Number of calling fork() : %d\n", findtask->fork_count);
    printk(KERN_INFO "- it's parent process : [%d] %s\n", findtask->parent->pid, findtask->parent->comm);
    
    //sibling process print
    printk(KERN_INFO "- it's sibling process(es) : \n");
    list_for_each_entry(task, &findtask->parent->children, sibling){
        //print if not same
	if(task->pid != findtask->pid){
            printk(KERN_INFO "\t> [%d] %s\n", task->pid, task->comm);
            count++;
        }
    }
    if(count == 0)  printk(KERN_INFO "\t> It has no sibling\n");
    else            printk(KERN_INFO "\t> This process has %d sibling process(es)\n", count);

    //child process print
    count = 0;
    printk(KERN_INFO "- it's child process(es) : \n");
    list_for_each_entry(task, &findtask->children, sibling){
        printk(KERN_INFO "\t> [%d] %s\n", task->pid, task->comm);
        count++;
    }
    if(count == 0)  printk(KERN_INFO "\t> It has no child\n");
    else            printk(KERN_INFO "\t> This process has %d child process(es)\n", count);

    printk(KERN_INFO "##### END OF INFORMATION #####\n");

    return pid;
}

//systemcall table get permission(Read Write)
void make_rw(void *addr){
    unsigned int level;
    pte_t *pte = lookup_address((u64)addr, &level);
    if(pte->pte &~ _PAGE_RW)
        pte->pte |= _PAGE_RW;
}

//systemcall table get permission(Read Only)
void make_ro(void *addr){
    unsigned int level;
    pte_t *pte = lookup_address((u64)addr, &level);
    
    pte->pte = pte->pte &~ _PAGE_RW;
}

//Module initialize Func.
static int __init process_tracer_init(void){
    syscall_table = (void**) kallsyms_lookup_name("sys_call_table");
    make_rw(syscall_table);
    real_ftrace = syscall_table[__NR_ftrace];
    syscall_table[__NR_ftrace] = process_tracer;
    make_ro(syscall_table);
    return 0;
}

//Module exit Func.
static void __exit process_tracer_exit(void){
    make_rw(syscall_table);
    syscall_table[__NR_ftrace] = real_ftrace;
    make_ro(syscall_table);
}

module_init(process_tracer_init);
module_exit(process_tracer_exit);
MODULE_LICENSE("GPL");
