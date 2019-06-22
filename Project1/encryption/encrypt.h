/*
	Operating System Project 1: Caesar Encryption Server
	Author: Zihan Wang    Student ID: 517021911179
	
	encrypt.h
	
	Common header for both server and client program.
	
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 1024

/* Convert current time to string. */
static char *time_str() {
	static char str[32];
	time_t rawtime;
    time(&rawtime);
    struct tm *ptm = localtime(&rawtime);
	sprintf(str, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return str;
}

