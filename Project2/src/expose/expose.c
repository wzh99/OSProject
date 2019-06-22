/*
	Operating System Project 2: Android Memory Management
	Author: Zihan Wang    Student ID: 517021911179
	
	expose.c
	
	expose_page_table is implemented in this file.
	It's a system call that expose the pages tables of a target process to current process.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");

#define NUM_SYSCALL 376

static int (*oldcall)();

int expose_page_table(pid_t pid, unsigned long fake_pgd, unsigned long fake_pmds, 
		unsigned long page_table_addr, unsigned long begin_vaddr, unsigned long end_vaddr) 
{
	printk("pid: %d fake_pgd: %lx fake_pmds: %lx page_table_addr: %lx begin_vaddr: %lx end_addr: %lx\n",
		pid, fake_pgd, fake_pmds, page_table_addr, begin_vaddr, end_vaddr);
	
	// Get task struct of target process
	struct pid *pid_ptr = find_get_pid(pid);
	if (pid_ptr == NULL) {
		printk("No such process with pid %d.\n", pid);
		return 1;
	}
	struct task_struct *task = pid_task(pid_ptr, PIDTYPE_PID);

	// Get VMA struct from address and set flags
	struct vm_area_struct *fake_pgd_vma = find_vma(current->mm, fake_pgd);
	if (fake_pgd_vma == NULL) {
		printk("Invalid fake pgd address.\n");
		return 2;
	}
	fake_pgd_vma->vm_flags |= VM_DONTEXPAND;
	printk("fake_pgd_vma vm_start: %lx vm_end: %lx \n", fake_pgd_vma->vm_start, fake_pgd_vma->vm_end);
		
	struct vm_area_struct *fake_pmds_vma = find_vma(current->mm, fake_pmds);
	if (fake_pmds_vma == NULL) {
		printk("Invalid fake pmd address.\n");
		return 3;
	}
	fake_pmds_vma->vm_flags |= VM_DONTEXPAND;
	printk("fake_pmds_vma vm_start: %lx vm_end: %lx \n", fake_pmds_vma->vm_start, 
		fake_pmds_vma->vm_end);
	
	struct vm_area_struct *remap_ptes_vma = find_vma(current->mm, page_table_addr);
	if (remap_ptes_vma == NULL) {
		printk("Invalid remapped pte address.\n");
		return 4;
	}
	remap_ptes_vma->vm_flags |= VM_DONTEXPAND;
	printk("remap_ptes_vma vm_start: %lx vm_end: %lx \n", remap_ptes_vma->vm_start, 
		remap_ptes_vma->vm_end);
		
	// Allocate kernel buffer space for fake pgd and pmd
	unsigned long *kernel_fake_pgd = kmalloc(PTRS_PER_PGD * sizeof(pgd_t), GFP_KERNEL);
	unsigned long *kernel_fake_pmd = kmalloc(PTRS_PER_PGD * sizeof(pmd_t), GFP_KERNEL);
	
	// Traverse all the vmas in the address space
	struct vm_area_struct *task_vma;
	int finished = 0;
	unsigned long last_pgd_val = -1;
	for (task_vma = task->mm->mmap; task_vma != NULL; task_vma = task_vma->vm_next) {
		if (finished) break;
		
		unsigned long vm_start = task_vma->vm_start, vm_end = task_vma->vm_end;
		printk("vm_start: %lx vm_end: %lx\n", vm_start, vm_end);
		
		unsigned long vm_addr;
		for (vm_addr = vm_start; vm_addr < vm_end; vm_addr += (1 << PGDIR_SHIFT)) {
			if (vm_addr < begin_vaddr) continue;
			if (vm_addr > end_vaddr) finished = 1;
			if (finished) break;
			
			pgd_t *pgd = pgd_offset(task->mm, vm_addr);
			unsigned long cur_pgd_val = pgd_val(*pgd);
			if (cur_pgd_val != last_pgd_val) { // a new pgd is found
				printk("new pgd: %lx \n", cur_pgd_val);
				
				// Get pfn of the page table the pgd points to
				unsigned long pfn = cur_pgd_val >> PAGE_SHIFT;
				struct page *pg = pfn_to_page(pfn);
				unsigned long pgd_idx = pgd_index(vm_addr);
				// a page table (2^11 bytes) should occupy a whole page (2^12 bytes)
				unsigned long remap_addr = 
					page_table_addr + (pgd_idx << (PGDIR_SHIFT - PAGE_SHIFT + 1)) * sizeof(pte_t);
					
				// Insert page containing page table to user memory space
				int err = vm_insert_page(remap_ptes_vma, remap_addr, pg);
				if (err < 0) {
					printk("Cannot insert pfn %lx to target address %lx. Error code: %d \n", 
						pfn, remap_addr, err);
				} else {
					// Add entry in fake pgd and fake pmd
					printk("insert pfn %lx to target address %lx \n", pfn, remap_addr);
					kernel_fake_pmd[pgd_idx] = remap_addr;
					kernel_fake_pgd[pgd_idx] = fake_pmds + pgd_idx * sizeof(pmd_t);
				}
				
				last_pgd_val = cur_pgd_val;
			}
		}
	} // end task_vma loop
	
	// Copy fake pgd and fake pmd to user space
	unsigned long remain;
	remain = copy_to_user(fake_pgd, kernel_fake_pgd, PTRS_PER_PGD * sizeof(pgd_t));
	if (remain > 0) 
		printk("%d bytes could not be copied to fake pgd \n");
	else
		printk("fake pgd copied from %lx \n", kernel_fake_pgd);
	kfree(kernel_fake_pgd);
	
	remain = copy_to_user(fake_pmds, kernel_fake_pmd, PTRS_PER_PGD * sizeof(pmd_t));
	if (remain > 0)
		printk("%d bytes could not be copied to fake pmds \n");
	else
		printk("fake pmd copied from %lx \n", kernel_fake_pmd);
	kfree(kernel_fake_pmd);
	
	return 0;
}

static int addsyscall_init() {
    unsigned long *syscall = (unsigned long *) 0xc000d8c4;
    oldcall = (int (*)()) (syscall[NUM_SYSCALL]);
    syscall[NUM_SYSCALL] = (unsigned long) expose_page_table;
    printk(KERN_INFO "Module 'expose' loaded. System call number: %d.\n", NUM_SYSCALL);
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

