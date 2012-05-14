#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 16

struct products{
	int buffer[BUFFER_SIZE];
	int readpos, writepos;
	sem_t sem_read;
	sem_t sem_write;
};

void init(struct products *b)
{
	sem_init(&b->sem_write, 0, BUFFER_SIZE -1);
	sem_init(&b->sem_read, 0, 0);
	b->readpos = 0;
	b->writepos =0;
}

void put(struct products *b, int data)
{
	sem_wait(&b->sem_write);
	b->buffer[b->writepos] = data;
	b->writepos++;
	if (b->writepos >= BUFFER_SIZE)
		b->writepos = 0;
	sem_post(&b->sem_read);
}

int get(struct products *b)
{
	int data;
	sem_wait(&b->sem_read);
	data = b->buffer[b->readpos];
	b->readpos++;
	if (b->readpos >= BUFFER_SIZE)
		b->readpos=0;
	sem_post(&b->sem_write);
	return data;
}

#define  OVER (-1)
struct products buffer;

void *producer(void *data)
{
	int n;
	for (n=0; n<10000; n++)
	{
		printf("%d--------------> \n", n);
		put(&buffer, n);
	}
	put(&buffer, OVER);
	return NULL;
}

void *consumer(void *data)
{
	int d;
	while (1)
	{
		d=get(&buffer);
		if (d == OVER)	break;
		printf("-------------------> %d\n", d);
	}
	return NULL;
}

int main(void)
{
	pthread_t th_a, th_b;
	void* retval;

	init(&buffer);
	pthread_create(&th_a, NULL, producer, 0);
	pthread_create(&th_b, NULL, consumer, 0);

	pthread_join(th_a, &retval);
	pthread_join(th_b, &retval);
	return 0;
}
