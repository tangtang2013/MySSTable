/******************************************
 * this is small and simple thread pool
 * may have some bug
 * time : 2013-05-24
 * copyleft by TT
 *****************************************/
#ifndef __SSTABLE_THREADPOOL_H_
#define __SSTABLE_THREADPOOL_H_

#include <stdio.h>
#include "common.h"

typedef struct jobNode
{
	void *(*func)(void*);	//call back function
	void *arg;				//the argument of the call back function
	struct jobNode *next;	//next point
}jobNode_t;

typedef struct  threadPool
{
	int threadNum;		//the number of threads
	int jobNum;			//the number of work job
	jobNode_t* jobQueue;//the queue of work job

	uv_cond_t cond;	//semaphore(not use...)
	uv_mutex_t mutex;		//like a mutex
	uv_thread_t *threadIDs;	//thread ID array

	int doShutDown;		//flag of shutdown 0/1
}threadPool_t;

threadPool_t* threadPool_new(int threadNum);
void threadPool_destroy(threadPool_t* pool);
void threadPool_init(threadPool_t* pool);
void threadPool_addJob(threadPool_t* pool, void *(*func)(void*), void* arg);

//thread excute function
void *func(void *arg);

#endif