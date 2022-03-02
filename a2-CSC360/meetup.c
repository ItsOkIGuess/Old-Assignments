/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */
 pthread_mutex_t m;
 pthread_mutex_t Writelock;
 pthread_mutex_t lock;
 pthread_cond_t cond;
 pthread_mutex_t readlock;
 resource_t meet;
 int numBobaFetts = 0;
 int gravy;
 int fm;
 int sand = 0;
int spinner = 0;
void initialize_meetup(int n, int mf) {
    char label[100];
    int i;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
    
        exit(1);
    }
	/*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
	gravy = n;
	pthread_cond_init(&cond, NULL);
	init_resource(&meet, label);
	fm = mf;
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&Writelock, NULL);
	pthread_mutex_init(&readlock, NULL);
	//pthread_mutex_init(&m, NULL);
}


void join_meetup(char *value, int len) {
	pthread_mutex_lock(&Writelock);
	sand++;
	int localsand = sand;
	
	
	if((localsand % gravy == 1 && fm == 1)|| (localsand % gravy ==0 && fm != 1)){
		pthread_mutex_lock(&lock);
		numBobaFetts = 0;
		write_resource(&meet,value,len);
		spinner = 1;
		
	}else{
		while(spinner == 1){
			
		}
	}
	spinner = 0;
	

	
	if(localsand % gravy != 0){
		printf("%d Is Waiting\n", localsand);
		
		pthread_cond_wait(&cond,&Writelock);
		printf("%d Is Done Waiting\n", localsand);
	}else{
		
		printf("%d Condidtion Brodcasted\n", localsand);
		for(int i = 0; i < 1000; i++){}
		pthread_cond_broadcast(&cond);
		
		
	}
	
	pthread_mutex_lock(&readlock);
	read_resource(&meet,value,len);
	numBobaFetts++;
	printf("%d\n",numBobaFetts);
	if(numBobaFetts == gravy){
		pthread_mutex_unlock(&lock);
	}
	pthread_mutex_unlock(&readlock);
	pthread_mutex_unlock(&Writelock);
}
