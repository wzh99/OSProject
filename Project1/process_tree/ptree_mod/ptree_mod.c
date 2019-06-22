/*
	Operating System Project 1: Android Process Tree
	Author: Zihan Wang    Student ID: 517021911179
	
	ptree_mod.c
	
	The ptree kernel module is implemented in this file.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include "../ptree.h"

MODULE_LICENSE("GPL");

static int (*oldcall)();

/* To omit 'struct task_struct' when using list entry function */
#define task_entry(list, member) list_entry(list, struct task_struct, member)

/* Recursively traverse tree and add entry to the buffer array. */
static void build_tree(struct prinfo *buf, int *nr, int limit, struct task_struct *task, int dep) {
	printk("At level %d.\n", dep);
	
	// Get index of current task
	int i = *nr;
	if (i >= limit)
		return; // buffer limit reached, no further copy
	
	// Copy task data into buffer
	printk("Task number: %d.\n", i);
	buf[i].parent_pid = task->real_parent->pid;
	buf[i].pid = task->pid;
	buf[i].state = task->state;
	buf[i].uid = task->real_cred->uid;
	buf[i].first_child_pid = task_entry(task->children.next, sibling)->pid;
	buf[i].next_sibling_pid = task_entry(task->sibling.next, sibling)->pid;
	strcpy(buf[i].comm, task->comm);
	buf[i].depth = dep;
	
	struct prinfo p = buf[i];
	printk("Task: %s, %d, %ld, %d, %d, %d, %d.\n", p.comm, p.pid, p.state, p.parent_pid, 
			p.first_child_pid, p.next_sibling_pid, p.uid);
			
	(*nr)++;
	
	// Traverse sibling and begin next depth of recursion
	printk("Traverse next level.\n");
	struct list_head *cursor;
	list_for_each(cursor, &task->children) {
		build_tree(buf, nr, limit, task_entry(cursor, sibling), dep + 1);
	}
	printk("Exit level %d.\n", dep);
}

static int ptree(struct prinfo *buf, int *nr) {
	printk(KERN_INFO "ptree() called.\n");
	printk("buf: %p, nr: %p\n", buf, nr);
	
	// Check validity of buf
	if (buf == NULL) {
		printk("Buffer pointer is null!\n");
		return NULL_BUFFER_POINTER;
	}
	
	// Check validity of nr
	if (nr == NULL) {
		printk("Buffer size pointer is null!\n");
		return NULL_SIZE_POINTER;
	}
	
	int limit = *nr;
	if (limit < 1) {
		printk("Invalid buffer size!\n");
		return INVALID_BUFFER_SIZE; 
	}
	
	*nr = 0; // clear counter of tasks
	
	printk("Ready to acquire task list lock.\n");
	read_lock(&tasklist_lock);
	
	printk("Start traversing task tree.\n");
	build_tree(buf, nr, limit, &init_task, 0);
	printk("Task tree traversed.\n");
	
	read_unlock(&tasklist_lock);
	printk("Task list lock released.\n");
	return 0;
}

static int addsyscall_init() {
    unsigned long *syscall = (unsigned long *) 0xc000d8c4;
    oldcall = (int (*)()) (syscall[NUM_SYSCALL]);
    syscall[NUM_SYSCALL] = (unsigned long) ptree;
    printk(KERN_INFO "Module loaded. System call number: %d.\n", NUM_SYSCALL);
    printk(KERN_INFO "Build %s, %s", __DATE__, __TIME__);
    return 0;
}

static void addsyscall_exit() {
    unsigned long *syscall = (unsigned long *) 0xc000d8c4;
    syscall[NUM_SYSCALL] = (unsigned long) oldcall;
    printk(KERN_INFO "Module exit. \n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

