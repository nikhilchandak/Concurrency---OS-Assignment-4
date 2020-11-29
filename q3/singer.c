#include "headers.h"

void performSolo(singer* sing, int stageType) {
    int stageNumber ; 
    int duration = rng(MIN_DURATION, MAX_DURATION);

    // find stage somehow!!!
    if(stageType) {
        sem_wait(&electricStageLock);
        stageNumber = elec[elecIndex--] ;
        sem_post(&electricStageLock);
    }
    else {
        sem_wait(&acousticStageLock);
        stageNumber = acous[acousIndex--] ;
        sem_post(&acousticStageLock);
    }

    prepareMsg(PERFORMING, sing->id, "%s performing solo at %s stage %d for %d seconds\n", sing->name, ( (stageType) ? "Electrical" : "Acoustic"), stageNumber, duration);

    // starts performing 
    sleep(duration);
    // print performance finished 

    prepareMsg(PERFORMED, sing->id, "%s's solo performance at %s stage %d ended\n", sing->name, ( (stageType) ? "Electrical" : "Acoustic"), stageNumber);

    // change number of stags accordingly 
    if(stageType == ACOUSTIC) {
        sem_post(&acousticStages);
        sem_wait(&acousticStageLock);
        acous[++acousIndex] = stageNumber ;
        sem_post(&acousticStageLock);
    }
    else {
        sem_post(&elecStages);
        sem_wait(&electricStageLock);
        elec[++elecIndex] = stageNumber ;
        sem_post(&electricStageLock);
    }
    sem_post(&emptyStages);

    // move for tshirt  
    collectShirt(sing->id, SINGER); 
}

void findStage(singer* sing) {
    int res ;
    res = sem_timedwait(&forSingers, sing->st) ; 

    if (res == 0) {

        if(sem_trywait(&emptyStages) == 0) { // use an empty stage
            if(sem_trywait(&acousticStages) == 0) performSolo(sing, ACOUSTIC);
            else if (sem_trywait(&elecStages) == 0) performSolo(sing, ELECTRICAL);
        }
        else { // join a instrumentalist already performing 
            sem_wait(&singerJoinLock);
            SINGER_ID = sing->id ;
            sem_post(&dualCount); 
        }

        return;
    } else if (res == ETIMEDOUT) {
        prepareMsg(LEFT, sing->id, "%s has left due to impatience!!\n", sing->name);
        return;
    } else {
        perror("DEBUG 5: ");
        return;
    }
}

void* handleSinger(void* arg) {
    singer* sing = (singer *)arg;
    sing->st = getTimeStructSinceEpoch(sing->arrivalTime + MAX_WAIT);

    sleep(sing->arrivalTime);
    prepareMsg(ARRIVAL, sing->id, "%s has arrived\n", sing->name);
    findStage(sing); 

    return NULL;
}

