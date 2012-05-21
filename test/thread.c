    #include <stdio.h>  
    #include <stdlib.h>  
    #include <pthread.h>  
    #include <unistd.h>
      
      
      
    pthread_cond_t taxiCond;   
      
     // 同步锁  
    pthread_mutex_t taxiMutex;   
      
     // 旅客到达等待出租车  
    void * traveler_arrive(void * name)   
    {   
        
        printf("Traveler: %s",(char*)name);  
        printf(" needs a taxi now !\n");  
        pthread_mutex_lock(&taxiMutex);   
        pthread_cond_wait (&taxiCond, &taxiMutex);   
        pthread_mutex_unlock (&taxiMutex);   
         
        printf("Traveler: %s",(char*)name);  
        printf(" now got a taxi!\n");  
        pthread_exit( (void *)0 );   
     }   
      
     // 出租车到达  
     void * taxi_arrive(void *name) {   
         
        printf("Taxi %s",(char *)name);  
        printf(" arrives.\n");    
        pthread_cond_signal(&taxiCond);   
        pthread_exit( (void *)0 );   
     }   
      
    int  main() {    
        // 初始化  
        taxiCond = PTHREAD_COND_INITIALIZER;   
        taxiMutex = PTHREAD_MUTEX_INITIALIZER;   
        pthread_t thread;   
        pthread_attr_t threadAttr;   
        pthread_attr_init(&threadAttr);   
      
        pthread_create(&thread, & threadAttr, taxi_arrive, (void *)( "Jack" ));   
        sleep(1);   
        pthread_create(&thread, &threadAttr, traveler_arrive, (void *)( "Susan " ));   
        sleep(1);   
        pthread_create(&thread, &threadAttr, taxi_arrive, (void *)( "Mike " ));   
        sleep(1);   
        system("pause");  
        return 0;   
     }  
	 
