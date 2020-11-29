#include "headers.h"

void collectSingerShirt(singer* sing) {
    sem_wait(&coordinatorsAvailable);
    // tshirt collected 
    sleep(2);
    prepareMsg(COLLECTING, sing->id, "%s collecting t-shirt\n", sing->name);
    sem_post(&coordinatorsAvailable);
}

void performSolo(singer* sing, int stageType) {

    int duration = rng(MIN_DURATION, MAX_DURATION);

    prepareMsg(PERFORMING, sing->id, "%s performing solo at %s stage for %d seconds\n", sing->name, ( (stageType) ? "Electrical" : "Acoustic"), duration);

    // starts performing 
    sleep(duration);
    // print performance finished 

    prepareMsg(PERFORMED, sing->id, "%s performance at %s stage ended\n", sing->name, ( (stageType) ? "Electrical" : "Acoustic"));

    // move for tshirt 
    collectSingerShirt(sing); 
}

void findStage(singer* sing) {
    int res ;
    while( (res = sem_timedwait(&forSingers, sing->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {

        if(sem_trywait(&emptyStages) == 0) {
            if(sem_trywait(&acousticStages) == 0) {
                performSolo(sing, ACOUSTIC);
                sem_post(&acousticStages);
            }
            else if (sem_trywait(&elecStages) == 0) {
                performSolo(sing, ELECTRICAL);
                sem_post(&elecStages);
            }
        }
        else {
            sem_wait(&singerJoinLock);
            SINGER_ID = sing->id ;
            sem_post(&dualCount); 
        }

        return;
    } else if (res == ETIMEDOUT) {
        prepareMsg(LEFT, sing->id, "has left due to impatience!!");
        return;
    } else {
        perror("DEBUG: ");
        return;
    }
}


void* handleSinger(void* arg) {
    singer* sing = (singer *)arg;
    sing->st = getTimeStructSinceEpoch(sing->arrivalTime + MAX_WAIT);

    sleep(sing->arrivalTime);
    printf("in singer!!!\n");
    prepareMsg(ARRIVAL, sing->id, "has arrived");
    printf("in singer!!!\n");
    findStage(sing); 

    return NULL;
}




