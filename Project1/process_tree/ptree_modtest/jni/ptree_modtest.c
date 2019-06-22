/*
	Operating System Project 1: Android Process Tree
	Author: Zihan Wang    Student ID: 517021911179
	
	ptree_modtest.c
	
	This program performs additional test on the 'ptree' system call
	to test this system call exit gracefully on invalid input.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../../ptree.h"
#include <errno.h>

/* Limits the size of prinfo buffer. */
#define BUFFER_SIZE 1024

/* 
	prinfo buffer array and entity count.
	These variables can only be defined in the global scope.
	malloc(), local variable and static variable will lead to addressing error.
*/
struct prinfo buf[BUFFER_SIZE];
int nr = BUFFER_SIZE;

void call_ptree(struct prinfo *buf, int *size) {
	int err = syscall(NUM_SYSCALL, buf, size);
	
	if (err != 0) {
		if (err < 0) // error caused by system
			printf("Error: %s.\n", strerror(errno));
		else if (err == NULL_BUFFER_POINTER) // error caused by invalid user input
			printf("Error: Buffer pointer is null!\n");
		else if (err == NULL_SIZE_POINTER)
			printf("Error: Buffer size pointer is null!\n");
		else if (err == INVALID_BUFFER_SIZE)
			printf("Error: Invalid buffer size!\n");
		else 
			printf("Unknown error!\n");
		return;
	}

	int ip;
	for (ip = 0; ip < *size; ip++) {
		struct prinfo p = buf[ip];
		int id;
		for (id = 0; id < p.depth; id++)
			printf("\t");
		printf("%s, %d, %ld, %d, %d, %d, %d\n", p.comm, p.pid, p.state, p.parent_pid, 
			p.first_child_pid, p.next_sibling_pid, p.uid);
	}
}

int main(int argc, char *argv[]) {
	printf("Call with null buffer pointer.\n");
	call_ptree(NULL, &nr);
	
	printf("Call with null size pointer.\n");
	call_ptree(buf, NULL);
	
	printf("Call with invalid size.\n");
	nr = -1;
	call_ptree(buf, &nr);
	
	printf("Call with limited size. \n");
	nr = 10;
	call_ptree(buf, &nr);
}
