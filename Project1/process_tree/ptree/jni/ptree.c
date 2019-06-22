/*
	Operating System Project 1: Android Process Tree
	Author: Zihan Wang    Student ID: 517021911179
	
	ptree.c
	
	The ptree test program is implemented in this file.
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

int main(int argc, char *argv[]) {
	int err = syscall(NUM_SYSCALL, buf, &nr);
	
	// In 'ptree' program, the arguments are sure to be valid, 
	// so there's no need to handle error caused by invalid user input
	if (err != 0) { // error caused by system
		printf("Error: %s.\n", strerror(errno));
		return err;
	}

	int ip;
	for (ip = 0; ip < nr; ip++) {
		struct prinfo p = buf[ip];
		int id;
		for (id = 0; id < p.depth; id++)
			printf("\t");
		printf("%s, %d, %ld, %d, %d, %d, %d\n", p.comm, p.pid, p.state, p.parent_pid, 
			p.first_child_pid, p.next_sibling_pid, p.uid);
	}
}
