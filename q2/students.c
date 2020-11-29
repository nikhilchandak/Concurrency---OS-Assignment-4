#include "headers.h" 



void slotAsignment(student* stud, zone* z) {
    printf("Student %d has been assigned a slot in zone %d\n", stud->id, z->id);
    int j = 0;
    while (z->studentsBeingVaccinated[j] != -1)
        j++;
    
    pthread_mutex_lock(&studentQueue);
    studentsInQueue-- ; 
    // printf("Current number of students in queue: %d\n", studentsInQueue);
    z->studentsBeingVaccinated[j] = stud->id;
    pthread_mutex_unlock(&studentQueue);

    pthread_mutex_unlock(mutexZone + z->id);
}


void waitSlot(student* stud) {
	printf("Student %d is waiting for an vaccination slot\n", stud->id);
	int i ; 
    while (1) {
        for (i = 0; i < zoneCount; i++) {
            pthread_mutex_lock(mutexZone + i);
            int found = 0;

            if (zones[i].readyToTreat && zones[i].slotsLeft > 0) {
                zones[i].slotsLeft--;
                found = 1;
            }
            if (found) {
                slotAsignment(stud, zones + i);
                return;
            }
            pthread_mutex_unlock(mutexZone + i);
        }
    }
}

void* handleStudent(void* arg) {
	student* stud = (student *) arg ; 

	stud->arrivalTime = rng(1, MAX_ARRIVAL_TIME);
	printf("Student %d initialized with arrival time %d\n", stud->id, stud->arrivalTime);
	sleep(stud->arrivalTime);
	printf("Student %d has arrived\n", stud->id);

	while(!stud->vaccinated && stud->vaccinationCount < MAX_VACCINATION_TIMES) {
		int vaccinationCount ;
		vaccinationCount = stud->vaccinationCount ; 

		pthread_mutex_lock(&studentQueue);
		printf("Student %d is going for his/her vaccination round %d\n", stud->id, vaccinationCount + 1);
		studentsInQueue++ ;
		pthread_mutex_unlock(&studentQueue);

		waitSlot(stud); 

		// while(stud->vaccinationCount == vaccinationCount) {} // wait till student gets vaccinated 
		while(1) {
			pthread_mutex_lock(mutexStudent + stud->id);
			if(stud->vaccinationCount > vaccinationCount) {
				// todo here anything??
				break ; 
			}
			pthread_mutex_unlock(mutexStudent + stud->id);
		}
		pthread_mutex_unlock(mutexStudent + stud->id);

		// now test student 
		printf("Student %d is about to get antibody test. Fingers crossed!\n", stud->id);
		float p = rng(0, RAND_MAX); p /= RAND_MAX ;
		float req = prob[stud->lastVaccineFrom] ;
		if(p <= req) {
			printf("Yay! Student %d has tested postive and can now attend college!\n", stud->id);
			stud->vaccinated = 1 ; 
		} 
		else 
			printf("Alas! Student %d has tested negative \n", stud->id);
	}
	if(!stud->vaccinated) printf("Unfortunately, student %d has failed all 3 anitbody tests and will be sent home for online semester :(\n", stud->id);

}