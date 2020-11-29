#ifndef headers
#define headers
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "colors.h"
#include <errno.h>

#define MAX_WAIT_TIME 1000
#define MAX_ARRIVAL_TIME 10
#define MAX_MUSICIANS 100
#define MAX_COORDINATORS 100
#define MAX_SINGERS 100
#define MAX_STAGES 100 
#define LEN 30
#define EXTRA_TIME 2 

enum states {ARRIVAL, PERFORMING, PERFORMED, COLLECTING, LEFT, JOINING};

#define ACOUSTIC 0
#define ELECTRICAL 1

sem_t serversOpen;
int serversOpenCount;
int totalCabsOpen;

sem_t coordinatorsAvailable;
sem_t emptyStages, elecStages, acousticStages, dualCount, forSingers;  
sem_t binary, mutex, singerJoinLock ;

int SINGER_ID ;

typedef struct musician {
    char instrument ; 
    char name[LEN] ; 
    int type;
    int maxWaitTime;
    int duartion;
    int id;
    int withSinger ;
    struct timespec* st;
    int arrivalTime;
    pthread_cond_t cond;
    short isWaiting;
} musician;

typedef struct singer {
    char name[LEN] ; 
    int maxWaitTime;
    int duartion;
    int id;
    struct timespec* st;
    int arrivalTime;
    pthread_cond_t cond;
    short isWaiting;
} singer;

typedef struct stage {
    int type, id;
    int player1, player2 ; 
} stage;

typedef struct coordinator {
    int id;
} coordinator ;

// stage electricalStages[MAX_STAGES] ; 
// pthread_t electricalThread[MAX_STAGES] ;
// pthread_mutex_t mutexElectrical[MAX_STAGES] ; 

// stage acousticStages[MAX_STAGES] ; 
// pthread_t acousticThread[MAX_STAGES] ;
// pthread_mutex_t mutexAcoustic[MAX_STAGES] ; 

singer singers[MAX_SINGERS] ; 
pthread_t singerThread[MAX_SINGERS] ;
pthread_mutex_t mutexSinger[MAX_SINGERS] ;

musician musicians[MAX_MUSICIANS] ; 
pthread_t musicianThread[MAX_MUSICIANS] ;
pthread_mutex_t mutexMusician[MAX_MUSICIANS] ; 

void* handleSinger(void *);
void* handleMusician(void *);
void* handleStage(void *); 
// void* handleCoordinator(void *); 

char* getTimestamp();
char* getHeader(int type, int id);
void printMsg(int type, int id, char* fmt, va_list arg);
void prepareMsg(int type, int id, char* fmt, ...);

int rng(int l, int r);
int coordinatorCount, musicianCount, singerCount, stageCount, instrumentalistCount;
int acousticCount, electricalCount ; 
int MIN_DURATION, MAX_DURATION, MAX_WAIT ; 
// int studentsInQueue ; pthread_mutex_t studentQueue ; 
int over ;

void printTimestamp();
void printMsg(int type, int id, char* fmt, va_list arg);
#endif