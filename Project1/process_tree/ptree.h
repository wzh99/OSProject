/*
	Operating System Project 1: Android Process Tree
	Author: Zihan Wang    Student ID: 517021911179
	
	ptree.h
	
	This is the common include header of kernel module and ptree program.
	This ensures the consistency of definition of struct and macros.
*/

#pragma once

#include <linux/types.h>

/* 
	Calling system call 391 will cause a SIGSYS (function not implemented) error on my platform.
	Therefore, I switch to another number that works.
*/
#define NUM_SYSCALL 375

struct prinfo {
	pid_t parent_pid;
	pid_t pid;
	pid_t first_child_pid;
	pid_t next_sibling_pid;
	long state;
	long uid;
	
	/* Modified member(s) */
	char comm[16]; // in accordance to the task_struct definition
	
	/* Newly added member(s) */
	int depth; // record depth of current process in the process tree
};

/* Define error caused by invalid user input */
#define NULL_BUFFER_POINTER 1
#define NULL_SIZE_POINTER   2
#define INVALID_BUFFER_SIZE 3


