Back to College 

How to run:
make -f makefile
./a.out

Info:

This is implemented via busy waiting and mutex. Files:

students.c 	- 	initializes student, waits to find an empty vaccination slot and goes for antibody test after vaccination
pharma.c 	-	prepares group of batches of vaccines which are collected by vaccination zones
zone.c 		-	Finds a free pharmaceutical, takes a vaccine batch from it and prepares slot for student and treats them.
headers.h	- 	Header file containing all the essential constants and function declarations
main.c 		- 	For helper functions and initialization of mutexes and threads 

Assumption:

n, m, o will be given greater than 0 otherwise it doesn't make sense to go back to college. 
Max input of each of n, m, o will be < 500. 