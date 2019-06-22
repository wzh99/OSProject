/*
	Operating System Project 1: Android Process Tree
	Author: Zihan Wang    Student ID: 517021911179
	
	ptree_exec.c
	
	The main program forks a child process and executes 'ptree' to show the relationship 
	between the parent and child processes.
	
*/

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	// Fork a child process
	printf("517021911179Parent, PID: %d.\n", getpid());
	pid_t pid = fork();
	
	if (pid < 0) { // error occured
		fprintf(stderr, "Fork failed.\n");
		return 1;
	} else if (pid == 0) { // child process
		printf("517021911179Child, PID: %d.\n", getpid());
		execlp("./ptree", "ptree", NULL);
	} else { // parent process
		wait(NULL);
		printf("Child complete.\n");
	}
	
	return 0;
	
}

