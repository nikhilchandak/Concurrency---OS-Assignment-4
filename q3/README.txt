Musical Mayhem

How to run:

make -f makefile
./a.out

Files:

headers.c 	- To include all the relevant libraries and do function/constant declarations
main.c 		- Inlcudes lots of helper functions (mostly for coloring purposes) and initializes semaphores and threads
musician.c  - Simulates a given musician (instrumentalist but not singers) throughout from arrival to performance to tshirt collection	
singer.c  	- Simulates a given singer throughout from arrival to performance to tshirt collection	

Assumption:

1. The maximum number of stages and performers are not more than 100.
2. The number of coordinators is at least 1. 

Logic:

I have used 5 semaphores primarily to simulate the main logic (non-bonus functionality):

1. electrical_empty - To keep track of electrical stages which are empty
2. acoustic_empty  	- To keep track of acoustic stages which are empty
3. empty_stages		- To keep track of empty stage. It is always the sum of electric_empty and acoustic_empty.

4. forSinger 		- To keep track of stages where a singer can perform (ie. empty stages and stages where only one person 											instrumentalist (non-singer) is performing)

5. dualCount 		- This semaphore is used in an untraditional manner. The instrumentalist performs a timed_wait on this semaphore
					  for the amount of duration of his performance. So, the performer kind of waits for a singer to join his performance
					  and if the singer joins the perfomance, then the duration of performance is increased by 2 seconds. Singer, on the hand,
					  is not able to find an empty stage then does a sem_post on this semaphore to indicate any live (single) performers of its presence. 

The coordinators are directly simulated by a semaphore.

To keep track of stages, I have used a global queue where a stage is pushed/popped when acquired by some performer while locking/unlocking 
it accordingly when needed using a binary semaphore.
