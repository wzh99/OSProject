/*
	Operating System Project 2: Android Memory Management
	Author: Zihan Wang    Student ID: 517021911179
	
	vm_inspector.c
	
	vm_inspector is implemented in this file.
	It's a program that dump the page table entries of a target process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define NUM_SYSCALL 376

#define PAGE_SHIFT 12
#define PMD_SHIFT 21
#define PGDIR_SHIFT 21

const static char *errstr[] = {
	"", // 0: No error
	"No such process", // 1
	"Invalid fake pgd address", // 2
	"Invalid fake pmd address", // 3
	"Invalid remapped pte address" // 4
};

const static char *usage = "Usage: vm_inspector #pid #va_begin #va_end";

int main(int argc, char *argv[]) {	
	// Check number of arguments
	if (argc != 4) {
		printf("%s\n", usage);
		return 1;
	}
	
	// Convert arguments to digits
	char *endptr;
	int pid = strtol(argv[1], &endptr, 10);
	unsigned long start_addr = strtoul(argv[2], &endptr, 16);
	unsigned long end_addr = strtoul(argv[3], &endptr, 16);
	
	// Allocate space for page tables
	unsigned long *fake_pgd = malloc((1 << 11) * sizeof(unsigned long));
	if (fake_pgd == NULL) {
		printf("malloc failed for fake_pgd.\n");
		return 1;
	}
	unsigned long *fake_pmds = malloc((1 << 11) * sizeof(unsigned long));
	if (fake_pmds == NULL) {
		printf("malloc failed for fake_pmds.\n");
		return 1;
	}
	unsigned long *remap_ptes = mmap(NULL, (1 << 21) * sizeof(unsigned long),
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (remap_ptes == MAP_FAILED) {
		printf("mmap failed for remap_ptes.\n");
		return 1;
	}
	
	// System call
	int err = syscall(NUM_SYSCALL, pid, fake_pgd, fake_pmds, remap_ptes, start_addr, end_addr);
	if (err != 0) {
		const char *str = (err > 0 && err <= 4) ? errstr[err] : "Unknown";
		printf("System call error: %s\n", str);
		return 1;
	}
	
	// Print page table entries
	printf("Virtual address\tPage table entry\n");
	unsigned long addr = start_addr & ~((1 << PAGE_SHIFT) - 1);
	for (; addr <= end_addr; addr += (1 << PAGE_SHIFT)) {
		unsigned long pgd_index = addr >> PGDIR_SHIFT;
		unsigned long pgd_val = fake_pgd[pgd_index];
		// printf("pgd_index: %lx pgd_val: %lx \n", pgd_index, pgd_val);
		if (pgd_val == 0) continue;
		unsigned long *pmd_addr = (unsigned long *) pgd_val;
		unsigned long pmd_val = pmd_addr[0];
		// printf("pmd_val: %lx \n", pgd_val);
		if (pmd_val == 0) continue;
		unsigned long *pte_addr = (unsigned long *) pmd_val;
		unsigned long pte_index = (addr >> PAGE_SHIFT) & ((1 << (PMD_SHIFT - PAGE_SHIFT)) - 1);
		unsigned long pte_val = pte_addr[pte_index];
		// printf("pte_index: %lx pte_val: %lx \n", pte_index, pte_val);
		if (pte_val == 0) continue;
		printf("%#.8lx\t%#.8lx\n", addr, pte_val);
	}
	
	// Clean up
	free(fake_pgd);
	free(fake_pmds);
	err = munmap(remap_ptes, (1 << 21) * sizeof(unsigned long));
	if (err != 0) {
		printf("munmap remap_ptes failed.\n");
		return 1;
	}
}

