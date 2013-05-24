/******************************************
 * this is small and simple thread pool
 * may have some bug
 * time : 2013-05-24
 * copyleft by TT
 *****************************************/
#ifndef __SSTABLE_THREADPOOL_H_
#define __SSTABLE_THREADPOOL_H_

#include <Windows.h>

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

	HANDLE semaphore;	//semaphore(not use...)
	HANDLE mutex;		//like a mutex
	HANDLE *threadIDs;	//thread ID array

	int doShutDown;		//flag of shutdown 0/1
}threadPool_t;

threadPool_t* threadPool_new(int threadNum);
void threadPool_destroy(threadPool_t* pool);
void threadPool_init(threadPool_t* pool);
void threadPool_addJob(threadPool_t* pool, void *(*func)(void*), void* arg);

//thread excute function
void *func(void *arg);

#endif