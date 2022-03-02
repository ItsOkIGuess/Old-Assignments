/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */
 int readers, writers,deadlockbreaker;
 sem_t R,W,WLock;


static resource_t data;

void initialize_readers_writer() {
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
	 readers,writers = 0;
	 deadlockbreaker = 1;
	 sem_init(&R, 0,0);
	 sem_init(&W, 0,0);
	 sem_init(&WLock, 0, 1);
	 init_resource(&data, "data");
}


void rw_read(char *value, int len) {
	readers++;
	if(writers > 0){
		sem_wait(&R);
	}
	
	read_resource(&data,value,len);
	readers--;
	sem_post(&W);
	
	if(writers == 0){
		sem_post(&R);
	}
}


void rw_write(char *value, int len) {
	writers++;
	if(readers > 0){
		sem_wait(&W);
	}
	write_resource(&data,value,len);
	writers--;
	sem_post(&R);

	if(readers == 0){
		sem_post(&W);
	}	
}
