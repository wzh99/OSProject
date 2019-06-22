/*
	Operating System Project 2: Android Memory Management
	Author: Zihan Wang    Student ID: 517021911179
	
	meminfo.c
	
	meminfo test program is implemented in this file.
	This program checks /proc/meminfo periodically and prints the sizes of active and inactive list.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DURATION 30
#define BUFFER_SIZE 64

int main(int argc, char *argv[]) {
	char buffer[BUFFER_SIZE];
	for (int t = 0; t < DURATION; t++) {
		// Open meminfo file
		FILE *info = fopen("/proc/meminfo","r");
		// Jump to line 6
		for (int i = 0; i < 5; i++)
			fgets(buffer, BUFFER_SIZE, info);
		int active, inactive;
		fscanf(info, "%s%d%s", buffer, &active, buffer);
		fscanf(info, "%s%d%s", buffer, &inactive, buffer);
		printf("%d %d\n", active, inactive);
		fclose(info);
		sleep(1);
	}
}
