/*
	Operating System Project 2: Android Memory Management
	Author: Zihan Wang    Student ID: 517021911179
	
	VATranslate.c
	
	VATranslate is implemented in this file.
	It's program that translate a virtual address to physical one.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

const static char *errstr[] = {
	"", // 0: No error
	"No such process", // 1
	"Invalid fake pgd address", // 2
	"Invalid fake pmd address", // 3
	"Invalid remapped pte address" // 4
};

const static char *usage = "Usage: VATranslate #pid #va";

struct pagetable_layout_info {
	unsigned long pgdir_shift;
	unsigned long pmd_shift;
	unsigned long page_shift;
};

int main(int argc, char *argv[]) {	
	// Check number of arguments
	if (argc != 3) {
		printf("%s\n", usage);
		return 1;
	}
	
	// Convert arguments to digits
	char *endptr;
	int pid = strtol(argv[1], &endptr, 10);
	unsigned long va = strtoul(argv[2], &endptr, 16);
	
	// System call: get_pagetable_layout
	struct pagetable_layout_info layout;
	int err = syscall(375, &layout, sizeof(struct pagetable_layout_info));
	// The argument of this system call does not depend on user input
	// and not library call is involved, so there is no need to check error
	const unsigned long pgdir_shift = layout.pgdir_shift;
	const unsigned long pmd_shift = layout.pmd_shift;
	const unsigned long page_shift = layout.page_shift;
	unsigned long offset_mask = (1 << page_shift) - 1;
	unsigned long page_mask = ~offset_mask;
	
	// Allocate space for page tables
	unsigned long *fake_pgd = malloc((1 << (32 - pgdir_shift)) * sizeof(unsigned long));
	if (fake_pgd == NULL) {
		printf("malloc failed for fake_pgd.\n");
		return 1;
	}
	unsigned long *fake_pmds = malloc((1 << (32 - pmd_shift)) * sizeof(unsigned long));
	if (fake_pmds == NULL) {
		printf("malloc failed for fake_pmds.\n");
		return 1;
	}
	unsigned long *remap_ptes = mmap(NULL, (1 << (32 - page_shift)) * sizeof(unsigned long) * 2,
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (remap_ptes == MAP_FAILED) {
		printf("mmap failed for remap_ptes.\n");
		return 1;
	}
	
	// System call: expose_page_table
	unsigned long begin_addr = va & page_mask;
	err = syscall(376, pid, fake_pgd, fake_pmds, remap_ptes, begin_addr, 
		begin_addr + (1 << page_shift));
	if (err != 0) {
		const char *str = (err > 0 && err <= 4) ? errstr[err] : "Unknown";
		printf("System call error: %s\n", str);
		return 1;
	}
	
	// Translate virtual address to physical address
	unsigned long pgd_index = va >> pgdir_shift;
	unsigned long pgd_val = fake_pgd[pgd_index];
	if (pgd_val == 0) {
		printf("Can't translate virtual address.\n");
		return 1;
	}
	unsigned long *pmd_addr = (unsigned long *) pgd_val;
	unsigned long pmd_index = (va >> pmd_shift) & ((1 << (pgdir_shift - pmd_shift)) - 1);
	unsigned long pmd_val = pmd_addr[pmd_index];
	if (pmd_val == 0) {
		printf("Can't translate virtual address.\n");
		return 1;
	}
	unsigned long *pte_addr = (unsigned long *) pmd_val;
	unsigned long pte_index = (va >> page_shift) & ((1 << (pmd_shift - page_shift)) - 1);
	unsigned long pte_val = pte_addr[pte_index];
	if (pte_val == 0) {
		printf("Can't translate virtual address.\n");
		return 1;
	}
	unsigned long pa = (va & offset_mask) | (pte_val & page_mask);
	printf("Physical adddress: %#.8lx\n", pa);
	
	// Clean up
	free(fake_pgd);
	free(fake_pmds);
	err = munmap(remap_ptes, (1 << (32 - page_shift)) * sizeof(unsigned long) * 2);
	if (err != 0) {
		printf("munmap remap_ptes failed.\n");
		return 1;
	}
}

