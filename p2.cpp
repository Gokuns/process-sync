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
#define ROAD_FILE "car.log"
using namespace std;
int pthread_sleep (int seconds);

#define NUM_THREADS 5 /* 4 different lanes + 1 PO */

struct Car {
  int id;
  struct timeval arrivalTime;
  struct timeval crossTime;
  int direction;
};
FILE *police_log_ptr;

pthread_mutex_t mutex;
pthread_cond_t conditionvar;
pthread_mutex_t road_mutex;
pthread_cond_t road_cond;

int snaptshotTime;
int sindex;



queue <Car> northQ;
queue <Car> eastQ;
queue <Car> southQ;
queue <Car> westQ;
int car_count=4;
int simulationTime = 5; //command line arg with -s
double p = 0.5; //command line arg with -p
struct timeval startTime; //start time of the simulation
struct timeval currentTime;
struct timeval northTimer;
int dirSelected= 0;
int dir = 0;
bool car_exist = true;
char lanes[4][6] = {"North", "East", "South", "West"};
queue <Car> *allLanes[4]= {&northQ, &eastQ, &southQ, &westQ};



void* road_function(void *lane)
{
  long thread_id = (long)lane;
  char direction[6];
  strcpy(direction, lanes[thread_id]);
  gettimeofday(&currentTime, NULL);



  while(startTime.tv_sec+simulationTime > currentTime.tv_sec){
    // printf("id is %ld , current time is %ld \n" , lane, currentTime.tv_sec);
      pthread_mutex_lock(&road_mutex);
    struct Car newcar;
    newcar.id=car_count;
    car_count++;
    newcar.arrivalTime=currentTime;
    newcar.direction=(int) ((long)lane);
    float probab = 0.0;
    if ((long) lane == 0){
      probab = 1-p;
    }else{
      probab=p;
    }

    if(northTimer.tv_sec+20<=currentTime.tv_sec && (long) lane == 0){
      // printf("----------------------------------Hasnt added stuf to north for 20 sec\n" );
      probab=1;
    }

    // probab=0.01;

    int randy = rand();
    float tl = (float)randy/RAND_MAX;
    // printf("The probab is %f of lane %d\n", tl, lane );
    if(tl < probab){

      gettimeofday(&currentTime, NULL);
      // printf("locked the intersection as lane %d\n", lane);
      switch((long)lane){
        case 0:
        // printf("North\n");
        northQ.push(newcar);
        gettimeofday(&northTimer, NULL);
        break;
        case 1:
        // printf("East\n");
        eastQ.push(newcar);
        break;
        case 2:
        // printf("South\n");
        southQ.push(newcar);
        break;
        case 3:
       // printf("West\n");
        westQ.push(newcar);
        break;

      }
      gettimeofday(&currentTime, NULL);
      // printf("After car addition!!!!!!!!!!!!!!!!!1 at time %ld \n", currentTime.tv_sec);
      // printf("\t%ld\n",northQ.size() );
      // printf("%ld\t\t%ld\n",westQ.size(), eastQ.size() );
      // printf("\t%ld\n", southQ.size());
        car_exist=true;
        pthread_cond_signal(&road_cond);
      pthread_mutex_unlock(&road_mutex);

     pthread_sleep(1);

    }else{
    pthread_mutex_unlock(&road_mutex);
    pthread_sleep(1);
    }
  }
  pthread_mutex_lock(&road_mutex);

  car_exist=true;
  pthread_mutex_unlock(&road_mutex);


  // printf("Lane %ld now signaling to exit\n", lane );
  pthread_cond_signal(&road_cond);
  // printf("now to exit\n" );

  pthread_exit(NULL);


}

void* po_function(void *lane)
{


  while (startTime.tv_sec+simulationTime > currentTime.tv_sec) {
    FILE* po_ptr = fopen(ROAD_FILE, "a+");
    pthread_mutex_lock (&road_mutex);
    gettimeofday(&currentTime, NULL);

    if(dirSelected==1)
    {
      if(northQ.size()>=5 || eastQ.size()>=5 || southQ.size()>=5 || westQ.size()>=5 )
      {
        dirSelected =0;
      }
    }
    if(currentTime.tv_sec - westQ.front().arrivalTime.tv_sec>=20 && westQ.size()>0){
      printf("%ld\n",currentTime.tv_sec - westQ.front().arrivalTime.tv_sec );
      printf("west is waiting for a long time\n");
      dir = 3;
      dirSelected = 1;
    }
    if(currentTime.tv_sec - southQ.front().arrivalTime.tv_sec>=20 && southQ.size()>0){
      printf("%ld\n",currentTime.tv_sec - southQ.front().arrivalTime.tv_sec );
      printf("south is waiting for a long time\n");
      dir = 2;
      dirSelected = 1;
    }
    if(currentTime.tv_sec - eastQ.front().arrivalTime.tv_sec>=20 && eastQ.size()>0){
      printf("%ld\n",currentTime.tv_sec - eastQ.front().arrivalTime.tv_sec );
      printf("east is waiting for a long time\n");
      dir = 1;
      dirSelected = 1;
    }
    if(currentTime.tv_sec - northQ.front().arrivalTime.tv_sec>=20 && northQ.size()>0){
      printf("north is waiting for a long time\n");
      dir = 0;
      dirSelected = 1;
    }
    if(dirSelected==0)
    {
      int sz=0;
      if(westQ.size()>=sz)
      {
        sz = westQ.size();
        dir =3;
      }
      if(southQ.size()>=sz)
      {
        sz = southQ.size();
        dir =2;

      }
      if(eastQ.size()>=sz)
      {
        sz = eastQ.size();
        dir =1;
      }
      if(northQ.size()>=sz)
      {
        sz = northQ.size();
        dir =0;

      }
      if(sz == 0){
        car_exist=false;
        // printf("no cars around at time %ld\n", currentTime.tv_sec);

police_log_ptr = fopen("police.log", "a+");
char buf[100];
strftime(buf,sizeof(buf), "%H:%M:%S", localtime(&currentTime.tv_sec));

fprintf(police_log_ptr, "%s\t Cell Phone\n", buf );
fclose(police_log_ptr);
        pthread_cond_wait(&road_cond, &road_mutex);
        // printf("Signal received that new car arrived at time %ld \n", currentTime.tv_sec);
        if((startTime.tv_sec+simulationTime > currentTime.tv_sec)){
        police_log_ptr = fopen("police.log", "a+");
        char bufy[100];
        strftime(bufy,sizeof(bufy), "%H:%M:%S", localtime(&currentTime.tv_sec));

        fprintf(police_log_ptr, "%s\t Honk\n",bufy);
        fclose(police_log_ptr);
      }else{
        police_log_ptr = fopen("police.log", "a+");
        char bufy[100];
        strftime(bufy,sizeof(bufy), "%H:%M:%S", localtime(&currentTime.tv_sec));

        fprintf(police_log_ptr, "%s\t Shift Over\n",bufy);
        fclose(police_log_ptr);
      }
        pthread_mutex_unlock(&road_mutex);

        pthread_sleep(3);
        // printf("Slpet for 3 secs at time %ld \n", currentTime.tv_sec);

        continue;
      }
      dirSelected = 1;
    }if(dirSelected==1)
    {
      queue <Car> selectedQ = *allLanes[dir];
      if(selectedQ.size()!=0){
            // gettimeofday(&currentTime, NULL);
        // printf("===================================\n at time %ld \n", currentTime.tv_sec);
        // printf("the crossing car is id:%d in %s lane\n", selectedQ.front().id, lanes[selectedQ.front().direction]);
        gettimeofday(&currentTime, NULL);
        struct Car selected = selectedQ.front();
        char buff[100];
        strftime(buff,sizeof(buff), "%H:%M:%S", localtime(&selected.arrivalTime.tv_sec));
        char currT[100];
        strftime(currT,sizeof(currT), "%H:%M:%S", localtime(&currentTime.tv_sec));
        fprintf(po_ptr,"%d\t%s\t\t%s\t%s\t%ld\n", selected.id, lanes[selected.direction], buff,currT, currentTime.tv_sec - selected.arrivalTime.tv_sec );
        selectedQ.pop();
        *allLanes[dir] = selectedQ;
        if(((startTime.tv_sec + snaptshotTime) <=currentTime.tv_sec) && ((startTime.tv_sec + snaptshotTime + 3) >currentTime.tv_sec)){
          printf("At %s:\n",currT);
          printf("\t%ld\n",northQ.size() );
          printf("%ld\t\t%ld\n",westQ.size(), eastQ.size() );
          printf("\t%ld\n", southQ.size());

        }



        pthread_mutex_unlock (&road_mutex);
        pthread_sleep(1);

      }else
      {

        dirSelected = 0;
        gettimeofday(&currentTime, NULL);
        pthread_mutex_unlock (&road_mutex);
      }

    }

    fclose(po_ptr);
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
  police_log_ptr = fopen("police.log", "w");
  fprintf(police_log_ptr, "Time \t\t Event\n");
  // fprintf(police_log_ptr, "_____ \t ____\n");
  fclose(police_log_ptr);



  long t1 = 0, t2 = 1, t3 = 2, t4 = 3;

  srand(500);

  struct Car araba;

  FILE* road_ptr = fopen(ROAD_FILE, "w");
  fprintf(road_ptr, "CarId\tDirection\tArrival-Time\tCross-Time\tWait-Time\n");
  fprintf(road_ptr, "==================================================================\n");
  fclose(road_ptr);

  for(int i = 1; i < argc; i++) { /* argv[0] is the program name */
    if(strcmp(argv[i], "-s") == 0) {
      simulationTime = atoi(argv[i+1]);
    } else if(strcmp(argv[i], "-p") == 0) {
      p = (double) atof(argv[i+1]);
    }else if (strcmp(argv[i], "-t") == 0) {
      snaptshotTime = atoi(argv[i+1]);
    }
  }

  printf("simulation time: %d\n",simulationTime);
  printf("probability: %f\n",p);
  gettimeofday(&startTime, NULL);
  gettimeofday(&northTimer, NULL);
  gettimeofday(&currentTime, NULL);

  char bufyy[100];
  strftime(bufyy,sizeof(bufyy), "%H:%M:%S", localtime(&startTime.tv_sec));

  printf("simulation starts at: %s\n", bufyy);
  int randy = rand();
  float tl = (float)randy/RAND_MAX;


  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&road_mutex, NULL);
  pthread_cond_init(&road_cond, NULL);

  struct Car araba1;
  araba1.id=0;
  araba1.direction = 0;
  araba1.arrivalTime = startTime;
  struct Car araba2;
  araba2.id=1;
  araba2.direction = 1;
  araba2.arrivalTime = startTime;
  struct Car araba3;
  araba3.id=2;
  araba3.direction = 2;
  araba3.arrivalTime = startTime;
  struct Car araba4;
  araba4.id=3;
  araba4.direction = 3;
  araba4.arrivalTime = startTime;


  northQ.push(araba1);
  eastQ.push(araba2);
  southQ.push(araba3);
  westQ.push(araba4);


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
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&road_mutex);
  pthread_cond_destroy(&road_cond);



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
  gettimeofday(&currentTime, NULL);

  //Upon successful completion, a value of zero shall be returned
  return res;

}
