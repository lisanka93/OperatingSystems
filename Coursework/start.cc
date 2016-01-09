
/* main.c */
# include "helper.h"

#define RAND_DIVISOR 100000000
#define TRUE 1


int main(int argc, char *argv[]) {

int _size = check_arg(argv[1]);          //size of queue

if (_size>500 || _size <0)               //errorcheck, maxsize = 500
{
 perror("invalid queue size");
 exit(1) ;
}

/*SHARED MEMORY*/
int shmid=shmget(SHM_KEY, SHM_SIZE, 0666|IPC_CREAT);           //creating memory space and return memory ID (neg if fails)
if(shmid < 0)
{
 perror("start shmget error");
 exit(1);
}

queue* data;
data = (queue*) shmat(shmid, (void*)0, 0);
if(data == (queue*)(-1))
{
 perror("start shmat error");
 exit(1);
}

data->size=_size;                                  //initialising the circular queue
data->front=-1;
data->end=-1;
data->numofconsumers=0;
printf("numofconsumers start%d\n", data->numofconsumers);

/*SEM ARRAY*/

int semSetid = semget(SEM_KEY, 3, 0666| IPC_CREAT);       //creating array of 3 semaphores
semSetid = sem_attach(SEM_KEY);

if(semSetid < 0)
{
 perror("Error creating semaphores");                       //error check creating sems
 exit(1);
}

/*SETTING UP SEMS*/

int bufferNotEmptySem = sem_init(semSetid, 0,0);                  //want to set value of semaphore to 0
if(sem_init(semSetid, 0, 0) < 0)
{
 perror("Error initialising full sem");
 exit(1);
}

int spaceSem = sem_init(semSetid,1, _size);
if(sem_init(semSetid, 1, _size) < 0)
{
 perror("Error initialising empty sem");
 exit(1);
}

int mutexSem = sem_init(semSetid, 2, 1);
if(sem_init(semSetid, 2, 1) < 0)
{
 perror("Error initialising mutex sem");
 exit(1);
}

return 0;
}
