#include "headers.h" 

int rng(int a, int b) { // [a, b]
	int dif = b - a + 1 ; 
	int rnd = rand() % dif ;
	return (a + rnd);
}

void generate() {
	pharmaCount = rng(1, 2);
	zoneCount = rng(1, 3);
	studentCount = rng(10, 30);
	printf("%d %d %d\n", pharmaCount, studentCount, zoneCount);

	int i ; 
	for(i = 0; i < pharmaCount ; i++) {
		prob[i] = rand() ; prob[i] /= RAND_MAX ; 
		// prob[i] /= 3.0 ;  
		printf("%f ", prob[i]); 
	}
	printf("\n"); 
}

void handleInput() {
	// generate();
	scanf("%d %d %d", &pharmaCount, &zoneCount, &studentCount);
	for(int i = 0; i < pharmaCount; i++) {
		scanf("%f", prob + i); 
	}
	if(pharmaCount == 0) {
		printf("No Pharmaceuticals open nearby. Semester will be online for all!\n");
		exit(0);
	}
	if(zoneCount == 0) {
		printf("No vaccination zones have been prepared. Semester will be online for all!\n");
		exit(0);
	}
	if(studentCount == 0) {
		printf("Wow, 0 students have opted to come to campus. Hence, semester will be online for all!\n");
		exit(0);
	}
}

int main() {
	srand(time(0));
	handleInput();

	// SET UP 
	int i ; 
	for(i = 0; i < pharmaCount; i++) {
		pharmas[i].id = i ;
		pharmas[i].groupNumber = 0 ;
		pthread_mutex_init(mutexPharma + i, NULL);
	}
	for(i = 0; i < zoneCount; i++) {
		zones[i].id = i ; 
		zones[i].currentPharma = -1 ;
		zones[i].readyToTreat = 0 ;
		pthread_mutex_init(mutexZone + i, NULL);
	}
	for(i = 0; i < studentCount; i++) {
		students[i].id = i ; 
		students[i].vaccinationCount = 0 ;
		students[i].vaccinated = 0 ; 
		students[i].lastVaccineFrom = -1 ; 
		pthread_mutex_init(mutexStudent + i, NULL);
	}
	over = 0 ; studentsInQueue = 0 ; pthread_mutex_init(&studentQueue, NULL); 
// pthread_create
	// Initialize threads 
	for(i = 0; i < pharmaCount; i++) pthread_create(&pharmaThread[i], NULL, handlePharma, pharmas + i);
	for(i = 0; i < zoneCount; i++) pthread_create(&zoneThread[i], NULL, handleZone, zones + i);
	for(i = 0; i < studentCount; i++) pthread_create(&studentThread[i], NULL, handleStudent, students + i); 

	for(i = 0; i < studentCount; i++) pthread_join(studentThread[i], NULL); 
	over = 1 ; assert(studentsInQueue == 0); 

	printf("Simulation over!\n");
	return 0; 
}