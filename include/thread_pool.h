#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include <map>
#include <pthread.h>

using namespace std;


typedef struct _thread_task
{
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	void (*func)(void *);
	void *args;
	bool bUsed;
}stThreadTask;

class threadPool
{
public:
	threadPool();
	threadPool(int n);
	virtual ~threadPool();

	int thread_add(void (*function)(void *), void *args);
	int thread_finish(pthread_t pid);
	int thread_destroy();
	static void* threadpool_handle(void *args);

//private:
public:
	#define DEFAULT_THREAD_NUM  1
	#define MAX_THREAD_NUM  8192
	pthread_mutex_t _mtx;
	pthread_cond_t _cond;
	int THREAD_NUM;
	int _usedThread;
	int THREAD_TOTAL;
	map<pthread_t, stThreadTask> _mThread;

private:
	void init(void);

};

#endif

