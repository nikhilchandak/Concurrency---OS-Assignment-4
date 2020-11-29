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
    strcat(buf, KNRM "");
    strcat(buf, getTimestamp());
    strcat(buf, "\t");

    switch (type) {
        case ARRIVAL:
            strcat(buf, KGREEN "Arrival:");
            break;
        case PERFORMING:
            strcat(buf, KMAGENTA "Performing:");
            break;
        case PERFORMED:
            strcat(buf, KBLUE "Performed:");
            break;
        case COLLECTING:
            strcat(buf, KYELLOW "Collecting:");
            break;
        case JOINING:
            strcat(buf, KCYAN "Joined:");
            break;
        case LEFT:
            strcat(buf, KRED "Left:");
            break;
    }

    char b2[10] = {0};

	sprintf(b2, " \t");
    if(type == LEFT) strcat(b2, "\t");

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

char getType(int c) {
	if(c == 0) return 'p';
	if(c == 1) return 'g';
	if(c == 2) return 'v';
	if(c == 3) return 'b' ;
	if(c == 4) return 's';
	return 'X' ; 
}

char* NAMES[] = {"Mack", "Amiee", "Arthur", "Jadwiga", "Betty", "Leonor", "Lorriane", "Ying", "Burma", "Breanna", "Tamiko", "Yun", "Janise", "Margarito", "Dodie", "Reba", "Eveline", "Kacy", "Hertha", "Cindy", "Kori", "Josue", "Tonja", "Usha", "Richard", "Kerrie", "Mariko", "Maryam", "Caleb", "Daryl", "Josefina", "Talisha", "Kiesha", "Willodean", "Shirleen", "Reyna", "Kassandra", "Jim", "Christa", "Celena", "Inge", "Zaida", "Rochel", "Geoffrey", "Ileana", "Terra", "Bernard", "Eulalia", "Berniece", "George"};

void generate() {
	instrumentalistCount = rng(5, 10);
	acousticCount = rng(0, instrumentalistCount / 3);
	electricalCount = rng(0, instrumentalistCount / 2);
	coordinatorCount = rng(1, 5);
	MIN_DURATION = rng(0, 20);
	MAX_DURATION = rng(MIN_DURATION, MIN_DURATION + 5);
	MAX_WAIT = rng(MIN_DURATION/2, MAX_DURATION/2);

	printf("%d %d %d %d %d %d %d\n", instrumentalistCount, acousticCount, electricalCount, coordinatorCount, MIN_DURATION, MAX_DURATION, MAX_WAIT);
	

	stageCount = electricalCount + acousticCount ;
	int i ; char name[LEN] ; char type ; int arrival ; 
	int m = 0, s = 0 ; int p = 0, q = 0 ;

	for(i = 0; i < instrumentalistCount; i++) {
		int idx = rng(1, 50) - 1 ;
		strcpy(name, NAMES[idx]);
		idx = rng(0, 4);
		type = getType(idx);
		arrival = rng(0, 10);

		// scanf("%s %c %d", name, &type, &arrival);
		printf("%s %c %d\n", name, type, arrival);

		int code = getCode(type);
		if(code == 4) {
			strcpy(singers[s].name, name);
			singers[s].id = p++ ;
			singers[s++].arrivalTime = arrival ; 
		}
		else {
			strcpy(musicians[m].name, name);
			musicians[m].id = q++ ; 
			musicians[m].arrivalTime = arrival ; 
			musicians[m].type = code ; 
			musicians[m].performanceFinished = 0 ; 
			musicians[m++].instrument = type ; 
		}
	}
	musicianCount = m ; singerCount = s ; 
	printf("\n");
}

void handleInput() {
	// generate(); 
	// return ; 
	scanf("%d %d %d %d %d %d %d", &instrumentalistCount, &acousticCount, &electricalCount, &coordinatorCount, &MIN_DURATION, &MAX_DURATION, &MAX_WAIT);
	stageCount = electricalCount + acousticCount ;
	int i ; char name[LEN] ; char type ; int arrival ; 
	int m = 0, s = 0 ; int p = 0, q = 0 ;

	for(i = 0; i < instrumentalistCount; i++) {
		scanf("%s %c %d", name, &type, &arrival);
		int code = getCode(type);
		if(code == 4) {
			strcpy(singers[s].name, name);
			singers[s].id = p++ ;
			singers[s++].arrivalTime = arrival ; 
		}
		else {
			strcpy(musicians[m].name, name);
			musicians[m].id = q++ ; 
			musicians[m].arrivalTime = arrival ; 
			musicians[m].type = code ; 
			musicians[m].performanceFinished = 0 ; 
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
	elecIndex = -1 ; acousIndex = -1 ; 
	for(i = 0; i < electricalCount; i++) elec[++elecIndex] = i + 1; 
	for(i = 0; i < acousticCount; i++) acous[++acousIndex] = i + 1 ; 

	sem_init(&coordinatorsAvailable, 0, coordinatorCount);
	
	sem_init(&elecStages, 0, electricalCount);
	sem_init(&acousticStages, 0, acousticCount);
	sem_init(&emptyStages, 0, electricalCount + acousticCount);

	sem_init(&dualCount, 0, 0);
	sem_init(&forSingers, 0, electricalCount + acousticCount);
	
	sem_init(&acousticStageLock, 0, 1); sem_init(&electricStageLock, 0, 1); 
	sem_init(&singerJoinLock, 0, 1); 

	// Initialize threads 
	for(i = 0; i < musicianCount; i++) pthread_create(&musicianThread[i], NULL, handleMusician, musicians + i);
	for(i = 0; i < singerCount; i++) pthread_create(&singerThread[i], NULL, handleSinger, singers + i); 

	for(i = 0; i < musicianCount; i++) pthread_join(musicianThread[i], NULL); 
	for(i = 0; i < singerCount; i++) pthread_join(singerThread[i], NULL); 
	over = 1 ; 
	
	printf("" KNRM);
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