#ifndef commonDone
#define commonDone

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
// #include "colors.h"

#define MAX_VACCINATION_TIMES 3 
#define MAX_SLOTS 8
#define MAX_ARRIVAL_TIME 20
#define MAX_PHARMA 500
#define MAX_STUDENTS 500
#define MAX_ZONE 500

#define PHARMA_TYPE 1
#define ZONE_TYPE 2
#define STUDENT_TYPE 3

typedef struct zone {
    int id;
    int vaccinesLeft;
    int currentPharma;
    int needVaccine;
    int readyToTreat;
    int slotsLeft;
    int phase ; 
    int studentsBeingVaccinated[MAX_SLOTS];
} zone;

typedef struct pharmaceutical {
    int id;
    int groupNumber;
    int batches;
    int capacity; // cap. of each batch 
    int lastMade ; int exhausted ; 
} pharmaceutical;

typedef struct student {
    int id;
    int arrivalTime;    
    int vaccinated; 
    int vaccinationCount ; 
    int lastVaccineFrom ;
} student;


zone zones[MAX_ZONE] ; 
pthread_t zoneThread[MAX_ZONE] ;
pthread_mutex_t mutexZone[MAX_ZONE] ; 

pharmaceutical pharmas[MAX_PHARMA] ; 
pthread_t pharmaThread[MAX_PHARMA] ;
pthread_mutex_t mutexPharma[MAX_PHARMA] ; 
float prob[MAX_PHARMA] ;

student students[MAX_STUDENTS] ; 
pthread_t studentThread[MAX_STUDENTS] ;
pthread_mutex_t mutexStudent[MAX_STUDENTS] ; 

void* handleStudent(void *);
void* handleZone(void *);
void* handlePharma(void *); 

// char* getTimestamp();
// char* getHeader(int type, int id);
// void printMsg(int type, int id, char* fmt, va_list arg);
int rng(int l, int r);
int pharmaCount, zoneCount, studentCount ;
int studentsInQueue ; pthread_mutex_t studentQueue ; 
int over ;

// cannot use such global variables because
// we are not allowed to use a global mutex
// int studentsLeftCount;

#endif