Operating System Project 2: Android Memory Management
Zihan Wang	517021911179

All the files included in this project are listed below.
Folders like 'jni' and files like 'Makefile', 'Android.mk' which repeatedly appear will not be explained here.

Note: Screen captures of test runs are included in the report and are explained in detail, so they will not appear as files in the directory.

project2
	src							# Sources of the all programs and system modules required in the project.
		expose					# A system call that expose page tables of a target process to the current process.
			expose.c 			# Source code of expose_page_table.
			Makefile
		kernel					# Modified kernel files are in this directory.
			internal.h 			# Common included header of kernel source files.
			mm_types.h 			# Linux header that define several types related to memory management.
			modification.txt	# Record modifications to the kernel source files. 
			swap.c 				# Linux source file where page swapping procedures are implemented.
			vmscan.c 			# Linux source file where page reclaiming procedures are implemented.
		layout					# A system call that output layout of page tables.
			layout.c 			# Source code of get_pagetable_layout.
			Makefile
		VATranslate 			# A program that translate a virtual address to physical one.
			jni
				Android.mk
				VATranslate 	# Source code of VATranslate.
		vm_inspector			# A program that dump the page tables entries of a target process.
			jni
				Android.mk
				vm_inspector.c 	# Source code of vm_inspector.
	test 						# Additional test programs not explicitly required in the project.
		meminfo					# A program that checks /proc/meminfo periodically.
			jni
				Android.mk
				meminfo.c 		# Source code of meminfo.
		pra_test 				# A program that occupies at most 1GB memory to test the page replacement algorithm.
			jni
				Android.mk
				pra_test.c 		# Source code of pra_test.
	Prj2README.txt				# This file.
	report.pdf					# Detailed report of this project.
