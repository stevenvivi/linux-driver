#include <stdio.h>
#include <stdlib.h>  
#include <unistd.h>  
#include <pthread.h>  
#include <time.h>  
#include <string.h>  
#define    BUFFER_SIZE 8

struct Products
{
	int buffer[BUFFER_SIZE];
	pthread_mutex_t locker;
	pthread_cond_t notEmpty;
	pthread_cond_t notFull;
	int posReadFrom;
	int posWriteTo;
};

struct Products products;

int BufferIsFull(struct Products* products)
{
	if ((products->posWriteTo + 1)% BUFFER_SIZE == products->posReadFrom)
	{
		return(1);
	}
	return(0);
}

int BufferIsEmpty(struct Products *products)
{
	if (products->posWriteTo == products->posReadFrom)
	{
		return(1);
	}
	return 0;
}


void Produce(struct Products* products, int item)  
{  
	pthread_mutex_lock(&products->locker); //原子操作  

	while (BufferIsFull(products))  
	{  
		pthread_cond_wait(&products->notFull, &products->locker);  
	} //无空间可写入  

	//写入数据  
	products->buffer[products->posWriteTo] = item;  
	products->posWriteTo++;  

	if (products->posWriteTo >= BUFFER_SIZE)  
	{  
		products->posWriteTo = 0;  
	}  
	pthread_cond_signal(&products->notEmpty);     //发信  
	pthread_mutex_unlock(&products->locker);    //解锁  
}  

int Consume(struct Products* products)  
{  
	int item;  

	pthread_mutex_lock(&products->locker);  

	while (BufferIsEmpty(products))  
	{  
		pthread_cond_wait(&products->notEmpty, &products->locker);  
	} //为空时持续等待,无数据可读  

	//提取数据  
	item = products->buffer[products->posReadFrom];  
	products->posReadFrom++;  

	if (products->posReadFrom >= BUFFER_SIZE) //如果到末尾,从头读取  
	{  
		products->posReadFrom = 0;  
	}  
	pthread_cond_signal(&products->notFull);   
	pthread_mutex_unlock(&products->locker);  

	return item;  
}  


#define END_FLAG (-1)  

struct Products products;  

void*   
ProducerThread(void *arg)  
{  
	int i;  
	for (i = 0; i < 16; i++)  
	{  
		printf("producer: %d\t", i);  
		Produce(&products, i);     
	}  
	Produce(&products, END_FLAG);  
	return NULL;  
}  

void*   
ConsumerThread(void *arg)  
{  
	int item;  

	while (1)  
	{  
		item = Consume(&products);  
		if (END_FLAG == item)  
			break;  

		printf("consumer: %d\t", item);  
	}  

	return (NULL);  
}  

void InitProducer(struct Products* products)  
{  
	memset(products->buffer,0,BUFFER_SIZE);  
	pthread_mutex_init(&products->locker, NULL);  
	products->notEmpty = PTHREAD_COND_INITIALIZER;  
	products->notFull = PTHREAD_COND_INITIALIZER;  
	products->posReadFrom = 0;  
	products->posWriteTo = 0;         
}  
int main(int argc, char* argv[])  
{               
	pthread_t producer;  
	pthread_t consumer;  
	void   *result ;  
	int  ret;      

	InitProducer(&products);     
	if ((ret= pthread_create(&producer, NULL, &ProducerThread, NULL)) != 0)  
	{  
		printf("producer 创建失败\n");     
	}  
	else  
	{  
		printf("producer 创建成功\n");    
	}  
	if ((ret= pthread_create(&consumer, NULL, &ConsumerThread, NULL) )!= 0)  
	{  
		printf("Consumer创建失败\n");     
	}  
	else  
	{  
		printf("Consumer 创建成功\n");    
	}    


	pthread_join(producer, &result);  
	pthread_join(consumer, &result);  

	system("pause");  
	exit(EXIT_SUCCESS);  
}  
