/*
	Operating System Project 2: Android Memory Mangement
	Author: Zihan Wang    Student ID: 517021911179
	
	layout.c
	
	get_pagetable_layout is implemented in this file.
	This system call output the layout of page tables in terms of page directory shifts.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");

#define NUM_SYSCALL 375

static int (*oldcall)();

struct pagetable_layout_info {
	uint32_t pgdir_shift;
	uint32_t pmd_shift;
	uint32_t page_shift;
};

int get_pagetable_layout(struct pagetable_layout_info *pgtbl_info, int size) {
	// Detect invalid input
	if (size < sizeof(struct pagetable_layout_info)) {
		printk("Invalid size for page table info.\n");
		return 1;
	}
	if (pgtbl_info == NULL) {
		printk("Page table info pointer is null.\n");
		return 2;
	}
	
	// Fill layout struct with number
	pgtbl_info->pgdir_shift = PGDIR_SHIFT;
	pgtbl_info->pmd_shift = PMD_SHIFT;
	pgtbl_info->page_shift = PAGE_SHIFT;
	
	return 0;
}

static int addsyscall_init() {
    unsigned long *syscall = (unsigned long *) 0xc000d8c4;
    oldcall = (int (*)()) (syscall[NUM_SYSCALL]);
    syscall[NUM_SYSCALL] = (unsigned long) get_pagetable_layout;
    printk(KERN_INFO "Module 'layout' loaded. System call number: %d.\n", NUM_SYSCALL);
    printk(KERN_INFO "Build %s, %s\n", __DATE__, __TIME__);
    return 0;
}

static void addsyscall_exit() {
    unsigned long *syscall = (unsigned long *) 0xc000d8c4;
    syscall[NUM_SYSCALL] = (unsigned long) oldcall;
    printk(KERN_INFO "Module exit. \n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);

