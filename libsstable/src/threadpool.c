#include "threadpool.h"

threadPool_t* threadPool_new(int threadNum)
{
	threadPool_t* pool = malloc(sizeof(threadPool_t));
	memset(pool,0,sizeof(threadPool_t));
	pool->threadNum = threadNum;

	//create Mutex/Semaphore Object
	pool->mutex = CreateMutex(NULL,FALSE,NULL);
	pool->semaphore = CreateSemaphore(NULL,0,2,NULL);

	return pool;
}

void threadPool_init(threadPool_t* pool)
{
	int i;
	pool->threadIDs = malloc(pool->threadNum * sizeof(HANDLE));
	
	for (i=0; i<pool->threadNum; i++)
	{
		pool->threadIDs[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)func,pool,0,NULL);
	}
}

void threadPool_destroy( threadPool_t* pool )
{
	int i;
	if (pool == NULL)
	{
		return;
	}
	printf("threadPool begin to destroy\n");
	while(pool->jobNum != 0)
	{
		NULL;	//not good, will cost lots of CPU
	}

	WaitForSingleObject(pool->mutex,INFINITE);	//change the threadPool Shutdown flag with TURE
	pool->doShutDown = TRUE;
	ReleaseMutex(pool->mutex);

	for (i=0; i<pool->threadNum; i++)
	{
		//TODO
		//wake up thread
	}
	
	for (i=0; i<pool->threadNum; i++)			//waiting for All thread complete run
	{
		WaitForSingleObject(pool->threadIDs[i],INFINITE);
	}
	
	CloseHandle(pool->mutex);					//close Object Handle and free
	CloseHandle(pool->semaphore);
	free(pool);
	pool = NULL;
}

void threadPool_addJob(threadPool_t* pool, void *(*func)(void*), void* arg )
{
	jobNode_t* head;
	jobNode_t* newJob = malloc(sizeof(jobNode_t));
	memset(newJob,0,sizeof(jobNode_t));

	newJob->func = func;							//create Work Job
	newJob->arg = arg;
	newJob->next = NULL;

	WaitForSingleObject(pool->mutex,INFINITE);		//add job to queue
	if (pool->jobQueue == NULL || pool->jobNum == 0)
	{
		pool->jobQueue = newJob;
	}
	else
	{
		head = pool->jobQueue;
		while (head->next != NULL)
		{
			head = head->next;
		}
		head->next = newJob;
	}
	pool->jobNum++;
	ReleaseMutex(pool->mutex);
	//Signal...
}

void * func( void *arg )
{
	jobNode_t* job;
	threadPool_t* pool = (threadPool_t*)arg;
	while (1)
	{
		WaitForSingleObject(pool->mutex,INFINITE);
		if (pool->doShutDown)								//the Pool will exit,so break while
		{
			ReleaseMutex(pool->mutex);
			ExitThread(999);
			break;
		}
		while (pool->jobNum == 0 && !pool->doShutDown)		//the work queue is empty, so wait for queue
		{
			ReleaseMutex(pool->mutex);
//			WaitForSingleObject(pool->semaphore,INFINITE);
			WaitForSingleObject(pool->mutex,INFINITE);
		}
		if (pool->doShutDown)
		{
			ReleaseMutex(pool->mutex);
			ExitThread(999);
			break;
		}
		job = pool->jobQueue;								//get job form queue
		pool->jobQueue = pool->jobQueue->next;
		pool->jobNum--;
		ReleaseMutex(pool->mutex);

		job->func(job->arg);								//execute the job
//		free(job->arg);										//free memory of job
		free(job);
		job = NULL;
	}
	ExitThread(0);
}