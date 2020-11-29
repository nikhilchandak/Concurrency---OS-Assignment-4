#include "headers.h" 

void prepareVaccine(pharmaceutical* pharma) {
	int prepareTime = rng(2, 5); // w 
	int batchCount = rng(1, 5); // r
	int batchCapacity = rng(10, 20); // p 

	// print 
	pharma->groupNumber++ ; 
	printf("Pharmaceutical %d will prepare group %d of vaccines for next %d seconds, producing %d batches of vaccines each with a capcity to treat %d students\n", pharma->id, pharma->groupNumber, prepareTime, batchCount, batchCapacity);

	sleep(prepareTime);

	pthread_mutex_lock(mutexPharma + pharma->id);
	pharma->lastMade = batchCount ; pharma->exhausted = 0 ; 
	pharma->batches = batchCount ; 
	pharma->capacity = batchCapacity ; 
	pthread_mutex_unlock(mutexPharma + pharma->id);
	printf("pharmaceutical %d is done preparing vaccine batches for group %d \n", pharma->id, pharma->groupNumber);
	// distribute(pharma);
}

void* handlePharma(void* arg) {
	pharmaceutical* pharma = (pharmaceutical *) arg ;
	printf("pharmaceutical %d initialized\n", pharma->id);
	while(!over) {
		prepareVaccine(pharma); 
		while(1) {
			pthread_mutex_lock(mutexPharma + pharma->id); 
			if(pharma->exhausted == pharma->lastMade) // all batches are exhausted now 
				break ; 
			pthread_mutex_unlock(mutexPharma + pharma->id);
		}
		pthread_mutex_unlock(mutexPharma + pharma->id);
		printf("All the vaccines prepared by Pharmaceutical %d are emptied. Resuming production now!\n", pharma->id); 
	}
}
