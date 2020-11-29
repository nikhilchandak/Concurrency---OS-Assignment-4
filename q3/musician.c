#include "headers.h"

void collectShirt(int id, int type) {
    sem_wait(&coordinatorsAvailable);
    // tshirt collected 
    sleep(2);

    if(type) prepareMsg(COLLECTING, id, "%s collecting t-shirt\n", singers[id].name);
    else prepareMsg(COLLECTING, id, "%s collecting t-shirt\n", musicians[id].name);

    sem_post(&coordinatorsAvailable);
}

void perform(musician* music, int stageType) {

    int res, id,stageNumber ;
    int duration = rng(MIN_DURATION, MAX_DURATION);
    struct timespec* st = getTimeStructSinceEpoch(duration);

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

    // starts performing 
    prepareMsg(PERFORMING, music->id, "%s performing %c at %s stage %d for %d seconds\n", music->name, music->instrument, ( (stageType) ? "Electrical" : "Acoustic"), stageNumber, duration);

    res = sem_timedwait(&dualCount, st) ;
    // while( (res = sem_timedwait(&dualCount, st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        // a singer has entered!! 

        id = SINGER_ID ; 
        prepareMsg(JOINING, id, "%s joined %s's performance, duration extended by 2 seconds\n", singers[id].name, music->name);

        sem_post(&singerJoinLock);
        // find which singer somehow 
        st->tv_sec += EXTRA_TIME ;
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, st, NULL); // wait for more performance to end
    } 
    else if (res == -1 && errno == ETIMEDOUT) {
    } 
    else {
        perror("DEBUG 1: ");
        return;
    }

    // print performance finished 
    if(res == 0) prepareMsg(PERFORMED, music->id, "%s and %s duet performance at %s stage %d ended\n", music->name, singers[id].name, ( (stageType) ? "Electrical" : "Acoustic"), stageNumber);
    else prepareMsg(PERFORMED, music->id, "%s's performance at %s stage %d ended\n", music->name, ( (stageType) ? "Electrical" : "Acoustic"), stageNumber);
    // add one more line about singer ?

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
    collectShirt(music->id, MUSICIAN); 
    if(res == 0) collectShirt(id, SINGER);
}

void findEmptyStage(musician* music) {
    int res ; 
    res = sem_timedwait(&emptyStages, music->st);
    // while( (res = sem_timedwait(&emptyStages, music->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        // printf("Entered %s\n", music->name); 
        if(sem_trywait(&acousticStages) == 0) perform(music, ACOUSTIC);
        else if(sem_trywait(&elecStages) == 0) perform(music, ELECTRICAL);
        else perror("Sem trywait");
        
        return;
    } else if (res == -1 && errno == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "%s has left due to impatience!!\n", music->name);
        return;
    } else {
        perror("DEBUG 2: ");
        return;
    }
}

void findAcousticStage(musician* music) {
    int res ; 
    res = sem_timedwait(&acousticStages, music->st);
    // while( (res = sem_timedwait(&acousticStages, music->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        int r = sem_wait(&emptyStages);
        if(r != 0) {
            perror("Error!!!");
        }
        perform(music, ACOUSTIC);

        return;
    } else if (res == -1 && errno == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "%s has left due to impatience!!\n", music->name);
        return;
    } else {
        perror("DEBUG 3: ");
        return;
    }
}

void findElectricalStage(musician* music) {

    int res ;
    res = sem_timedwait(&elecStages, music->st);

    if (res == 0) {
        int r = sem_wait(&emptyStages);
        if(r != 0) {
            perror("Error!!!");
        }
        perform(music, ELECTRICAL);

        return;
    } else if (res == -1 && errno == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "%s has left due to impatience!!\n", music->name);
        return;
    } else {
        perror("DEBUG 4: ");
        return;
    }
}


void* handleMusician(void* arg) {
    musician* music = (musician *)arg;
    music->st = getTimeStructSinceEpoch(music->arrivalTime + MAX_WAIT);

    sleep(music->arrivalTime);
    prepareMsg(ARRIVAL, music->id, "%s has arrived\n", music->name);
    
    if(music->type <= 1) findEmptyStage(music);
    else if(music->type == 2) findAcousticStage(music);
    else findElectricalStage(music);

    return NULL;
}
