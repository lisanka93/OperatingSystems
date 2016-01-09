// Producer

# include "helper.h"

using namespace std;

int main (int argc, char *argv[])
{
int shmid = shmget(SHM_KEY, SHM_SIZE, 0666|IPC_CREAT);       //attaches shared memory
if(shmid < 0)
{
 perror("producer shmget error");
 exit(1);
}

int semSetid = sem_attach(SEM_KEY);          //attaches to whole sem array
if(semSetid < 0)
{
 perror("Error creating semaphores");
 exit(1);
}

time_t start = time(NULL);                  //setting up the time
time_t finish;
int elapsed;

queue* data;
data = (queue*) shmat(shmid, (void*)0, 0);
if(data == (queue *)(-1))
{
 perror("producer shmat error");
 exit(1);
}

int producerID = check_arg(argv[1]);

if (check_arg(argv[1]) == -1)
{
 perror("producerID has to be integer");
 exit(1);
}

int numberofjobs = check_arg(argv[2]);

if (check_arg(argv[2]) == -1)
{
 perror("has to be at least one job!");
 exit(1);
}

int currentjob=0;

while(currentjob<numberofjobs)
{
 int joblength=2+rand()%6;                         //how long it will sleep

if(data->front==-1)
{
 data->front = 0;
}

//produce item
data->end = (data->end+1)%(data->size);

sem_wait(semSetid, 1);                    //wait until there is space in the queue, down sem 1 by one
sem_wait(semSetid, 2);                        //mutex waits, noone write in the queue

finish = time(NULL);
elapsed = (int)finish - (int)start;
printf("Producer(%d) time %d: Job id %d duration %d\n", check_arg(argv[1]), elapsed, data->end+1, joblength);

data->job[data->end].id= data->end +1;    //so if job number 1 it sits on location 0 of array, job jumber 2 in array location [1]
data->job[data->end].duration=joblength;


sem_signal(semSetid, 2);       // since left, mutex is up
sem_signal(semSetid, 0);      //signals that it added a job and there is a job in the queue, increase non empty by one

sleep(2+rand()%3);                              //will sleep until next job
currentjob++;
}

if(currentjob == numberofjobs)
{
 time_t finish2 = time(NULL);
 int elapsed2 = (int)finish2 - (int)start;
 printf ("Producer(%d) time %d: No more jobs to generate\n", producerID, elapsed2);
}
 sleep(60);       //so that doesnt reset immediately and consumes stuff from the queue

 return 0;
}
