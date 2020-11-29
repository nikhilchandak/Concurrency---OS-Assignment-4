#include "headers.h" 

int rng(int a, int b) { // [a, b]
	int dif = b - a + 1 ; 
	int rnd = rand() % dif ;
	return (a + rnd);
}

void printTimestamp() {
    time_t t;
    time(&t);
    char *t2 = ctime(&t);

    char buf[9] = {0};
    memcpy(buf, t2 + 11, 8);
    printf("%s\t", buf);
}

char *getTimestamp() {
    time_t t;
    time(&t);
    char *t2 = ctime(&t);

    char *buf = (char *)calloc(sizeof(char), 9);
    memcpy(buf, t2 + 11, 8);
    return buf;
}

char *getHeader(int type, int id) {
    char *buf = (char *)calloc(sizeof(char), 1000);
    strcat(buf, getTimestamp());
    strcat(buf, "\t");

    switch (type) {
        case ARRIVAL:
            strcat(buf, KGREEN "Arrival: ");
            break;
        case PERFORMING:
            strcat(buf, KMAGENTA "Performing: ");
            break;
        case PERFORMED:
            strcat(buf, KBLUE "Performed: ");
            break;
        case COLLECTING:
            strcat(buf, KYELLOW "Collecting: ");
            break;
        case JOINING:
            strcat(buf, KCYAN "Joined: ");
            break;
        case LEFT:
            strcat(buf, KRED "Left: ");
            break;
    }

    char b2[10] = {0};
    // alignment woes
    if (type == PERFORMING || type == PERFORMED || type == COLLECTING)
        sprintf(b2, " %d\t" KNRM, id + 1);
    else
        sprintf(b2, " %d\t\t" KNRM, id + 1);
    strcat(buf, b2);
    return buf;
}

void printMsg(int type, int id, char *fmt, va_list arg) {
    char *buf = (char *)calloc(sizeof(char), 1000);
    char buf2[1000] = {0};
    vsprintf(buf, fmt, arg);

    strcat(buf2, getHeader(type, id));
    strcat(buf2, buf);

    printf("%s", buf2);
}

void prepareMsg(int type, int id, char* fmt, ...) {
	return ; 
    va_list argptr;
    va_start(argptr, fmt);
    printMsg(type, id, fmt, argptr);
    va_end(argptr);
}

struct timespec* getTimeStructSinceEpoch(int extraTime) {
    struct timespec* st = (struct timespec*)malloc(sizeof(struct timespec*));

    // https://stackoverflow.com/q/46018295/
    clock_gettime(CLOCK_REALTIME, st);
    st->tv_sec += extraTime;

    return st;
}

int getCode(char c) {
	if(c == 'p') return 0;
	if(c == 'g') return 1;
	if(c == 'v') return 2 ;
	if(c == 'b') return 3;
	if(c == 's') return 4 ;
	return -1 ; 
}

void generate() {
}

void handleInput() {
	generate();
	scanf("%d %d %d %d %d %d %d", &instrumentalistCount, &acousticCount, &electricalCount, &coordinatorCount, &MIN_DURATION, &MAX_DURATION, &MAX_WAIT);
	stageCount = electricalCount + acousticCount ;
	int i ; char name[LEN] ; char type ; int arrival ; 
	int m = 0, s = 0 ; 

	for(i = 0; i < instrumentalistCount; i++) {
		scanf("%s %c %d", name, &type, &arrival);
		int code = getCode(type);
		if(code == 4) {
			pthread_mutex_init(mutexSinger + i, NULL);
			strcpy(singers[s].name, name);
			singers[s].id = i ;
			singers[s++].arrivalTime = arrival ; 
		}
		else {
			pthread_mutex_init(mutexMusician + i, NULL); 
			strcpy(musicians[m].name, name);
			musicians[m].id = i ; 
			musicians[m].arrivalTime = arrival ; 
			musicians[m].type = code ; 
			musicians[m++].instrument = type ; 
		}
	}
	musicianCount = m ; singerCount = s ; 
}

int main() {
	srand(time(0));
	handleInput();

	// SET UP 
	int i ; 

	// for(i = 0; i < electricalCount; i++) {
	// 	electricalStages[i].id = i ;
	// 	electricalStages[i].player1 = electricalStages[i].player2 = -1;
	// 	pthread_mutex_init(mutexElectrical + i, NULL);
	// }
	// for(i = 0; i < acousticCount; i++) {
	// 	acousticStages[i].id = i ;
	// 	acousticStages[i].player1 = acousticStages[i].player2 = -1;
	// 	pthread_mutex_init(mutexAcoustic + i, NULL);
	// }

	sem_init(&coordinatorsAvailable, 0, coordinatorCount);
	
	sem_init(&elecStages, 0, electricalCount);
	sem_init(&acousticStages, 0, acousticCount);
	sem_init(&emptyStages, 0, electricalCount + acousticCount);

	sem_init(&dualCount, 0, 0);
	sem_init(&forSingers, 0, electricalCount + acousticCount);
	
	// sem_init(&binary, 0, 1); sem_init(*mutex, 0, 1); 
	sem_init(&singerJoinLock, 0, 1); 


	printf("Initializing threads!!\n");

	// Initialize threads 
	for(i = 0; i < musicianCount; i++) pthread_create(&musicianThread[i], NULL, handleMusician, musicians + i);
	for(i = 0; i < singerCount; i++) pthread_create(&singerThread[i], NULL, handleSinger, singers + i); 

	printf("Waiting for threads to join %d %d \n", musicianCount, singerCount); 

	for(i = 0; i < musicianCount; i++) pthread_join(musicianThread[i], NULL); 
	for(i = 0; i < singerCount; i++) pthread_join(singerThread[i], NULL); 
	over = 1 ; 

	printf("Finished!\n");
	return 0; 
}

/*
5 1 1 4 2 10 5
Tanvi p 0
Sudh b 1
Manas g 0
Sriya v 1
Pahun s 1
*/