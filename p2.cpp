/*KUSIS ID: 53940 PARTNER NAME: Asli Karahan
KUSIS ID: 54040 PARTNER NAME: Gökalp Ünsal
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include <queue>
using namespace std;
int pthread_sleep (int seconds);

#define NUM_THREADS 5 /* 4 different lanes + 1 PO */

struct Car {
  int id;
  struct timeval arrivalTime;
  struct timeval crossTime;
  int direction;
};

pthread_mutex_t mutex;

queue <Car> northQ;
queue <Car> eastQ;
queue <Car> southQ;
queue <Car> westQ;
int car_count=4;
int simulationTime = 5; //command line arg with -s
double p = 0.5; //command line arg with -p
struct timeval startTime; //start time of the simulation
struct timeval currentTime;
int dirSelected= 0;
int dir = 0;

char lanes[4][6] = {"North", "East", "South", "West"};
queue <Car> *allLanes[4]= {&northQ, &eastQ, &southQ, &westQ};


void* road_function(void *lane)
{
  long thread_id = (long)lane;
  char direction[6];
  strcpy(direction, lanes[thread_id]);
  gettimeofday(&currentTime, NULL);

  while(startTime.tv_sec+simulationTime > currentTime.tv_sec){
    printf("id is %ld , current time is %ld \n" , lane, currentTime.tv_sec);
    printf("Locking the intersection\n");

  pthread_mutex_lock(&mutex);
  printf("locked the intersection as lane %d\n", lane);
  switch((long)lane){
    case 0:
    printf("North\n");

    struct Car newcar;
    newcar.id=car_count;
    car_count++;
    newcar.arrivalTime=currentTime;
    northQ.push(newcar);
    break;
    case 1:
    printf("East\n");

    break;
    case 2:
    printf("South\n");

    break;
    case 3:
    printf("West\n");

    break;

  }
  pthread_mutex_unlock(&mutex);
  printf("unlocked the intersection as lane %d\n", lane);
  //  pthread_sleep(1);
    gettimeofday(&currentTime, NULL);

  }
  pthread_exit(NULL);


}

void* po_function(void *lane)
{

  while (startTime.tv_sec+simulationTime > currentTime.tv_sec) {

      pthread_mutex_lock (&mutex);
    if(dirSelected==0)
    {
    int sz=0;
    if(westQ.size()>=sz)
    {
      sz = westQ.size();
      dir =3;
      //  printf("size of westQ: %d\n", westQ.size());
    }
    if(southQ.size()>=sz)
    {
      sz = southQ.size();
      dir =2;
      //  printf("size of southQ: %d\n", southQ.size());
    }
    if(eastQ.size()>=sz)
    {
      sz = eastQ.size();
      dir =1;
    //  printf("size of eastQ: %d\n", eastQ.size());
    }
    if(northQ.size()>=sz)
    {
      sz = northQ.size();
      dir =0;
      // printf("size of northQ: %d\n", northQ.size());
    }
    dirSelected = 1;
  }if(dirSelected==1)
  {
    queue <Car> selectedQ = *allLanes[dir];
    if(northQ.size()>=5 || eastQ.size()>=5 || southQ.size()>=5 || westQ.size()>=5 )
    {
    dirSelected =0;
          printf("hi\n");
    continue;

    }
    else if(selectedQ.size()!=0){
      printf("======================================");
      printf("the crossing car is id:%d in %s lane\n", selectedQ.front().id, lanes[selectedQ.front().direction]);
      selectedQ.pop();
      *allLanes[dir] = selectedQ;
      pthread_sleep(1);
      gettimeofday(&currentTime, NULL);
    }else
    {
      dirSelected = 0;
    }

  }

  pthread_mutex_unlock (&mutex);
  // pthread_mutex_destroy(&mutex);
  }
  pthread_exit(NULL);

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

  srand(500);

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
  gettimeofday(&startTime, NULL);
  printf("%f\n", startTime.tv_sec);
  int randy = rand();
  float tl = (float)randy/RAND_MAX;


pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
pthread_mutex_init(&mutex, NULL);
struct Car araba1;
araba1.id=130;
araba1.direction = 0;
struct Car araba2;
araba2.id=132;
araba2.direction = 0;
struct Car araba3;
araba3.id=133;
araba3.direction = 3;
struct Car araba4;
araba4.id=134;
araba4.direction = 3;
struct Car araba5;
araba5.id=135;
araba5.direction = 3;
struct Car araba6;
araba6.id=136;
araba6.direction = 2;
struct Car araba7;
araba7.id=137;
araba7.direction = 2;

northQ.push(araba1);
northQ.push(araba2);
westQ.push(araba3);
westQ.push(araba4);
westQ.push(araba5);
southQ.push(araba6);
southQ.push(araba7);



pthread_create(&thread_N, &attr, road_function, (void *)t1);
pthread_create(&thread_E, &attr, road_function, (void *)t2);
pthread_create(&thread_S, &attr, road_function, (void *)t3);
pthread_create(&thread_W, &attr, road_function, (void *)t4);



pthread_create(&thread_PO, &attr, po_function, NULL);


pthread_join(thread_N, NULL);
pthread_join(thread_E, NULL);
pthread_join(thread_S, NULL);
pthread_join(thread_W, NULL);

pthread_join(thread_PO, NULL);

printf("Latest@!!@#!@  %d\n", northQ.size());
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
