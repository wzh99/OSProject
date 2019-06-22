/*
	Operating System Project 2: Android Memory Management
	Author: Zihan Wang    Student ID: 517021911179
	
	pra_test.c
	
	This program occupies at most 1GB memory to test the page replacement algorithm.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define DURATION 15

int main(int argc, char *argv[]) {
	// map 1GB memory
	long *mem = mmap(NULL, (1 << 28) * sizeof(long),
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED) {
		printf("mmap failed.\n");
		return 1;
	}
	
	// Occupy memory space
	for (int i = 0; i < (1 << 18); i++)
		mem[i << 10] = 1; // write to every page
	
	// unmap memory
	int err = munmap(mem, (1 << 28) * sizeof(long));
	if (err != 0) {
		printf("munmap failed.\n");
		return 1;
	}
}
