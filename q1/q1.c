#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int id ; 
int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     id = shm_id ;
     return (int*)shmat(shm_id, NULL, 0);
}

#define N 1000000
int tmp[N] ;

void merge(int* arr, int l1, int r1, int l2, int r2) {
    int k = 0 ; int i = l1, j = l2 ; 
    
    // printf("%d %d %d %d \n", l1, r1, l2, r2);
    assert(l2 == (r1 + 1));

    while(i <= r1 && j <= r2) {
        if(arr[i] < arr[j]) tmp[k++] = arr[i++] ;
        else tmp[k++] = arr[j++] ; 
    }
    while(i <= r1) tmp[k++] = arr[i++] ;
    while(j <= r2) tmp[k++] = arr[j++] ; 

    int z = 0; 
    for(z = 0; z < k; z++) arr[l1 + z] = tmp[z] ; 
}

void selectionSort(int *arr, int l, int r) {
     for (int i = l; i < r; i++) {
          int minIdx = i ;
          for (int j = i+1; j <= r; j++)
               if (arr[j] < arr[minIdx]) 
                    minIdx = j ; 
          swap(arr + minIdx, arr + i); 
     }
}

void normal_mergesort(int *arr, int low, int high){
     int l = low, r = high ;

     // insertion sort
     if (r - l + 1 <= 5) {
        selectionSort(arr, l, r);
        return;
     }

     int mid = (r - l)/2 ; mid += l ; 
     if(low<high){
          normal_mergesort(arr, low, mid);
          normal_mergesort(arr, mid + 1, high);
          merge(arr, l, mid, mid + 1, r); 
     }
}

void mergesort(int *arr, int low, int high){
     int l = low, r = high ;

     // insertion sort
     if (r - l + 1 <= 5) {
        selectionSort(arr, l, r);
        return;
     }

     int mid = (r - l)/2 ; mid += l ; 
     if(low<high){
          int pid1=fork();
          int pid2;
          if(pid1==0){
               mergesort(arr, low, mid);
               _exit(1);
          }
          else{
               pid2=fork();
               if(pid2==0){
                    mergesort(arr, mid + 1, high);
                    _exit(1);
               }
               else{
                    int status;
                    waitpid(pid1, &status, 0);
                    waitpid(pid2, &status, 0);
                    merge(arr, l, mid, mid + 1, r); 
               }
          }
          return;
     }
}

struct arg{
     int l;
     int r;
     int* arr;
};

void *threaded_mergesort(void *a) {
     // note that we are passing a struct to the threads for simplicity.
     struct arg *args = (struct arg *)a;

     int l = args->l;
     int r = args->r;
     int *arr = args->arr;
     if (l > r)
        return NULL;

     // insertion sort
     if (r - l + 1 <= 5) {
        selectionSort(arr, l, r);
        return NULL;
     }

     int mid = (r - l)/2 ; mid += l ; 

     // sort left half array
     struct arg a1;
     a1.l = l;
     a1.r = mid;
     a1.arr = arr;
     pthread_t tid1;
     pthread_create(&tid1, NULL, threaded_mergesort, &a1);

     // sort right half array
     struct arg a2;
     a2.l = mid + 1;
     a2.r = r;
     a2.arr = arr;
     pthread_t tid2;
     pthread_create(&tid2, NULL, threaded_mergesort, &a2);

     // wait
     pthread_join(tid1, NULL);
     pthread_join(tid2, NULL);

     merge(arr, l, mid, mid + 1, r); 
}

void runSorts(long long int n){

     struct timespec ts;

     //getting shared memory
     int *arr = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++) {
          // scanf("%d", arr+i);
          arr[i] = rand() % N ; 
     }

     int brr[n+1], crr[n+1];
     for(int i=0;i<n;i++) {
          brr[i] = arr[i];
          crr[i] = arr[i] ;
     }

     printf("Running concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort
     mergesort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     pthread_t tid;
     struct arg a;
     a.l = 0;
     a.r = n-1;
     a.arr = crr;
     printf("Running threaded_concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multithreaded mergesort
     pthread_create(&tid, NULL, threaded_mergesort, &a);
     pthread_join(tid, NULL);
     for(int i=0; i<n; i++){
          printf("%d ",a.arr[i]);
     }
     printf("\n");

     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;

     printf("Running normal_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     // normal mergesort
     normal_mergesort(brr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",brr[i]);
     }
     printf("\n");

    // for(int i = 0; i < n; i++) {
    //     assert(arr[i] == a.arr[i]);
    //     assert(a.arr[i] == brr[i]);
    //     assert(a.arr[i] == crr[i]); 
    // }

     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t3 = en - st;

     printf("normal_mergesort ran:\n\t[ %Lf ] times faster than concurrent_mergesort\n\t[ %Lf ] times faster than threaded_concurrent_mergesort\n\n\n", t1/t3, t2/t3);
     shmdt(arr);
     
     shmctl(id, IPC_RMID, NULL); 
     return;
}

int main(){
     srand(time(0));

     long long int n = 1;
     scanf("%lld", &n);
     runSorts(n);
     // for(int i = 1; i < 21; i++) {
     //      printf("N = %lld\n", n);
     //      runSorts(n);
     //      printf("\n");
     //      n *= 2 ; 
     //      if(n >= N) break ; 
     // }
     return 0;
}
