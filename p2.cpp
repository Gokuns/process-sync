/*KUSIS ID: 53940 PARTNER NAME: Asli Karahan
KUSIS ID: 54040 PARTNER NAME: Gökalp Ünsal
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <queue>
#include <string.h>

using namespace std;

#define NUM_THREADS 5 /* 4 different lanes + 1 PO */

struct Car {
  int id;
  time_t arrivalTime;
  time_t crossTime;
};

pthread_mutex_t northMutex;
pthread_mutex_t eastMutex;
pthread_mutex_t southMutex;
pthread_mutex_t westMutex;

queue <Car> northQ;
queue <Car> eastQ;
queue <Car> southQ;
queue <Car> westQ;

int simulationTime; //command line arg with -s
double p; //command line arg with -p
time_t startTime; //start time of the simulation
time_t currentTime;

char lanes[4][6] = {"North", "East", "South", "West"};


void* road_function(void *lane)
{
  long thread_id = (long)lane;
  //char direction[5] = lanes[thread_id];

}

void* po_function(void *lane)
{

}




 /******************************************************************************
  pthread_sleep takes an integer number of seconds to pause the current thread
  original by Yingwu Zhu
  updated by Muhammed Nufail Farooqi
  *****************************************************************************/

int pthread_sleep (int seconds)
{
   pthread_mutex_t mutex;
   pthread_cond_t conditionvar;
   struct timespec timetoexpire;
   if(pthread_mutex_init(&mutex,NULL))
    {
      return -1;
    }
   if(pthread_cond_init(&conditionvar,NULL))
    {
      return -1;
    }
   struct timeval tp;
   //When to expire is an absolute time, so get the current time and add //it to our delay time
   gettimeofday(&tp, NULL);
   timetoexpire.tv_sec = tp.tv_sec + seconds; timetoexpire.tv_nsec = tp.tv_usec * 1000;

   pthread_mutex_lock (&mutex);
   int res =  pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
   pthread_mutex_unlock (&mutex);
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&conditionvar);

   //Upon successful completion, a value of zero shall be returned
   return res;

}


int main(int argc, char* argv[])
{
  pthread_t thread_N;
  pthread_t thread_E;
  pthread_t thread_S;
  pthread_t thread_W;

  pthread_t thread_PO;
  pthread_attr_t attr;

  long t1 = 0, t2 = 1, t3 = 2, t4 = 3;


  struct Car araba;

  for(int i = 1; i < argc; i++) { /* argv[0] is the program name */
  if(strcmp(argv[i], "-s") == 0) {
    simulationTime = atoi(argv[i+1]);
  } else if(strcmp(argv[i], "-p") == 0) {
    p = (double) atof(argv[i+1]);
  }
}

printf("simulation time: %d\n",simulationTime);
printf("probability: %f\n",p);

pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
pthread_mutex_init(&northMutex, NULL);
pthread_mutex_init(&eastMutex, NULL);
pthread_mutex_init(&southMutex, NULL);
pthread_mutex_init(&westMutex, NULL);


pthread_create(&thread_N, &attr, road_function, (void *)t1);
pthread_create(&thread_E, &attr, road_function, (void *)t2);
pthread_create(&thread_S, &attr, road_function, (void *)t3);
pthread_create(&thread_W, &attr, road_function, (void *)t4);

pthread_create(&thread_PO, &attr, po_function, NULL);



}
