#include "headers.h"

void collectShirt(musician* music) {
    sem_wait(&coordinatorsAvailable);
    // tshirt collected 
    sleep(2);

    prepareMsg(COLLECTING, music->id, "%s collecting t-shirt\n", music->name);
    sem_post(&coordinatorsAvailable);
}

void perform(musician* music, int stageType) {
    printf("Error!\n"); 

    int res ;
    int duration = rng(MIN_DURATION, MAX_DURATION);
    struct timespec* st = (struct timespec *) getTimeStructSinceEpoch(duration);

    // find stage somehow!!! (NO)

    // starts performing 
    prepareMsg(PERFORMING, music->id, "%s performing %c at %s stage for %d seconds\n", music->name, music->instrument, ( (stageType) ? "Electrical" : "Acoustic"), duration);

    while( (res = sem_timedwait(&dualCount, st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        // a singer has entered!! 

        int id = SINGER_ID ; 
        prepareMsg(JOINING, music->id, "%s joined %s's performance, duration extended by 2 seconds\n", singers[id].name, music->name);

        sem_post(&singerJoinLock);
        // find which singer somehow 
        st->tv_sec += EXTRA_TIME ;
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, st, NULL); // wait for more performance to end
    } 
    else if (res == ETIMEDOUT) {
    } 
    else {
        perror("DEBUG: ");
        return;
    }

    // print performance finished 
    prepareMsg(PERFORMED, music->id, "%s performance at %s stage ended\n", music->name, ( (stageType) ? "Electrical" : "Acoustic"));
    // add one more line about singer ?

    // move for tshirt 
    collectShirt(music); 
}

void findEmptyStage(musician* music) {
    printf("S");
    int res = sem_timedwait(&emptyStages, music->st);
    printf("X");
    // while( (res = sem_timedwait(&emptyStages, music->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
    printf("Error!\n"); 

        if(sem_trywait(&acousticStages) == 0) {
            perform(music, ACOUSTIC);
            sem_post(&acousticStages);
        }
        else if(sem_trywait(&elecStages) == 0) {
            perform(music, ELECTRICAL);
            sem_post(&elecStages);
        }
        else {
            perror("Sem trywait");
        }
        sem_post(&emptyStages);
        return;
    } else if (res == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "has left due to impatience!!");
        return;
    } else {
        perror("DEBUG: ");
        return;
    }
}

void findAcousticStage(musician* music) {
    printf("S");
    int res = sem_timedwait(&acousticStages, music->st);
    printf("X");
    // while( (res = sem_timedwait(&acousticStages, music->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        // riderPrintMsg(rider->id,
        //               "got signal that suitable cab is available\n");
        perform(music, ACOUSTIC);
        sem_post(&acousticStages);
        return;
    } else if (res == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "has left due to impatience!!");        
        return;
    } else {
        perror("DEBUG: ");
        return;
    }
}

void findElectricalStage(musician* music) {

    int res ;
    while( (res = sem_timedwait(&elecStages, music->st) == -1 && errno == EINTR) ) continue ;

    if (res == 0) {
        perform(music, ELECTRICAL);
        sem_post(&elecStages);
        return;
    } else if (res == ETIMEDOUT) {
        prepareMsg(LEFT, music->id, "has left due to impatience!!");
        return;
    } else {
        perror("DEBUG: ");
        return;
    }
}



void* handleMusician(void* arg) {
    musician* music = (musician *)arg;
    // printf("in music 11 !!!\n");
    music->st = getTimeStructSinceEpoch(music->arrivalTime + MAX_WAIT);

    // printf("in music 2222  !!!\n");

    sleep(music->arrivalTime);
    prepareMsg(ARRIVAL, music->id, "Musician %d has arrived\n", music->id);
    
    // printf("%d\n", music->type);

    if(music->type <= 1) findEmptyStage(music);
    else if(music->type == 2) findAcousticStage(music);
    else findElectricalStage(music);

    return NULL;
}




