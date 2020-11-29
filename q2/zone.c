#include "headers.h"

#define min(x, y) ((x) < (y) ? (x) : (y))


void treatStudents(zone* vaccinationZone) {
	int i ; 
    for (i = 0; i < MAX_SLOTS; i++)
        vaccinationZone->studentsBeingVaccinated[i] = -1;

	pthread_mutex_lock(mutexZone + vaccinationZone->id);
	
	// pthread_mutex_lock(studentQueue); 
	int slots = min(MAX_SLOTS, min(vaccinationZone->vaccinesLeft, studentsInQueue));
	vaccinationZone->slotsLeft = slots ; 
	if(slots < 1) {
		pthread_mutex_unlock(mutexZone + vaccinationZone->id);
		return ; 
	}
	// pthread_mutex_unlock(studentQueue);
	
	vaccinationZone->readyToTreat = 1 ; 
	printf("Zone %d has prepared %d slots and will begin vaccination phase soon\n", vaccinationZone->id, slots);
	pthread_mutex_unlock(mutexZone + vaccinationZone->id);

	while(1) { // wait till ALL slots are filled or NO students remain to be treated *CURRENTLY*
		pthread_mutex_lock(mutexZone + vaccinationZone->id);
		pthread_mutex_lock(&studentQueue);
		// printf("Zone %d has %d slot yet to be filled and %d students in general queue\n", vaccinationZone->id, vaccinationZone->slotsLeft, studentsInQueue);
		if(vaccinationZone->slotsLeft == 0 || studentsInQueue == 0) {
				
			break ; 
		}
		pthread_mutex_unlock(&studentQueue);
		pthread_mutex_unlock(mutexZone + vaccinationZone->id);
	}
	pthread_mutex_unlock(&studentQueue);

	vaccinationZone->readyToTreat = 0 ; 
	for(i = 0; i < MAX_SLOTS; i++) {
		int stud = vaccinationZone->studentsBeingVaccinated[i] ;
		if(stud == -1) break ;

		printf("Zone %d about to treat stduent %d\n", vaccinationZone->id, stud); 
		pthread_mutex_lock(mutexStudent + stud);
		vaccinationZone->vaccinesLeft-- ; 
		students[stud].vaccinationCount++ ; students[stud].lastVaccineFrom = vaccinationZone->currentPharma ; 
		printf("Student %d has been vaccinated at zone %d with success probability %f\n", stud, vaccinationZone->id, prob[vaccinationZone->currentPharma]);
		pthread_mutex_unlock(mutexStudent + stud); 
	}

	for(i = 0; i < MAX_SLOTS; i++) vaccinationZone->studentsBeingVaccinated[i] = -1 ; 
	pthread_mutex_unlock(mutexZone + vaccinationZone->id); 
}


void* handleZone(void* arg) {
	zone* vaccinationZone = (zone *) arg ; 
	printf("Zone %d initiated\n", vaccinationZone->id);

	int i ; 
	while(1) {
		int found = 0 ; 
		for(i = 0; i < pharmaCount; i++) {

			pthread_mutex_lock(mutexPharma + i);
			if(pharmas[i].batches > 0 && vaccinationZone->currentPharma == -1) {
				found = 1 ;
				printf("Zone %d will begin obtaining vaccines from Pharmaceutical %d\n", vaccinationZone->id, i);
				pharmas[i].batches-- ;
				pthread_mutex_lock(mutexZone + vaccinationZone->id);
				vaccinationZone->vaccinesLeft = pharmas[i].capacity ;
				vaccinationZone->currentPharma = i ; 
				printf("Zone %d has finished obtaining %d vaccines from Pharmaceutical %d\n", vaccinationZone->id, vaccinationZone->vaccinesLeft, i);
				pthread_mutex_unlock(mutexZone + vaccinationZone->id);

			}
			pthread_mutex_unlock(mutexPharma + i); 

			if(found) break;
		}

		if(found) {
			while(1) {
				pthread_mutex_lock(mutexZone + vaccinationZone->id);
				int cnt = vaccinationZone->vaccinesLeft ; 
				pthread_mutex_unlock(mutexZone + vaccinationZone->id);
				if(cnt > 0) {
					vaccinationZone->phase++ ; 
					treatStudents(vaccinationZone); 
				}
				else {
					// sleep(10); 
					printf("Vaccination zone %d has run out of vaccines!!\n", vaccinationZone->id);
					pharmas[vaccinationZone->currentPharma].exhausted++ ; 
					vaccinationZone->currentPharma = -1 ;
					vaccinationZone->phase = 0 ; 
					break ;
				} 
			}
		}


	}

}