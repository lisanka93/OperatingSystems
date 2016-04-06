// Consumer

# include "helper.h"

using namespace std;


int main (int argc, char *argv[])
{


int shmid = shmget(SHM_KEY, SHM_SIZE, 0666|IPC_CREAT);
if(shmid < 0)
{
perror("consumer shmget");
exit(1);
}

queue* data;
data = (queue*) shmat(shmid, (void*)0, 0);
if(data == (queue *)(-1))
{
 perror("consumer shmat");
 exit(1);
}

time_t start = time(NULL);
int elapsed;

int semSetid =sem_attach(SEM_KEY);
int consumerID = check_arg(argv[1]);

int item = 0;
/*printf("numofconsumers before incr%d\n", data->numofconsumers);
data->numofconsumers++;
printf("numofconsumers after incr%d\n", data->numofconsumers);*/

while(sem_timewait(semSetid, 0, 10) != -1)
{
sem_wait(semSetid,2 );                     //mutex waits, noone write in the queue

item = data->job[data->front].id;
data->front = (data->front+1)%(data->size);

int duration = data->job[data->end].duration;
time_t finish = time(NULL);
elapsed = (int)finish - (int)start;

printf("Consumer(%d) time %d: Job id %d executing sleep duration %d\n", check_arg(argv[1]), elapsed, item, duration);

sem_signal(semSetid,2 );
sem_signal(semSetid,1 );

sleep(duration);

finish = time(NULL);
elapsed = (int)finish - (int)start;

printf("Consumer(%d) time %d: Job id %d completed.\n", check_arg(argv[1]), elapsed, item);

}

time_t finish2 = time(NULL);
int elapsed2 = (int)finish2 - (int)start;

printf("Consumer(%d) time %d: No jobs left.\n", check_arg(argv[1]), elapsed2);
/*
printf("numofconsumers before decr%d\n", data->numofconsumers);
data->numofconsumers--;
printf("numofconsumers after decr %d\n", data->numofconsumers);*/

if(data->numofconsumers==0){
if(sem_close(semSetid) < 0)
{
 perror("sem_close");
 exit(1);
}

if(shmdt(data) == -1)
{
 perror("shmdt");
 exit(1);
}

shmctl(shmid, IPC_RMID, NULL);
printf("program finished, semaphores destroyed.");
return 0;
}

sleep(15);

return 0;
}
