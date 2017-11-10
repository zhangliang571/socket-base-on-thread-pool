

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include "thread_pool.h"



void* threadPool::threadpool_handle(void *args)
{
	pthread_t pid;
	threadPool *th;
	map<pthread_t, stThreadTask>::iterator itm;
	if(args == NULL)
		return NULL;

	pid = pthread_self();
	th = static_cast<threadPool*>(args);

	// wait for _mThread
	for(;;)
	{
		pthread_mutex_lock(&(th->_mtx));
		itm = th->_mThread.find(pid);
		pthread_mutex_unlock(&(th->_mtx));
		
		if(itm == th->_mThread.end())
		{
			cout<<pid<<" no finded\n";
			sleep(1);
		}
		else
			break;
	}


	for(;;)
	{
		pthread_mutex_lock(&(itm->second.mtx));


		//cout<<pid<<" handle waiting signal.....\n";
		// wait cond signal
		pthread_cond_wait(&(itm->second.cond), &(itm->second.mtx));
		//cout<<pid<<" handle waited signal.....\n";


		// begin work
		itm->second.func(itm->second.args);
		itm->second.bUsed = false;

		pthread_mutex_unlock(&(itm->second.mtx));
		usleep(100);
		th->thread_finish(pid);	
	}

	return NULL;
}


threadPool::threadPool()
{
	THREAD_NUM = DEFAULT_THREAD_NUM;
	_usedThread = 0;
	init();
}
threadPool::threadPool(int n)
{
	THREAD_NUM = n>MAX_THREAD_NUM?MAX_THREAD_NUM:n;
	_usedThread = 0;
	THREAD_TOTAL = 0;
	init();
}
threadPool::~threadPool()
{
	pthread_mutex_destroy(&_mtx);	
	pthread_cond_destroy(&_cond);	
	_mThread.clear();
}

void threadPool::init(void)
{
	if(pthread_mutex_init(&_mtx, NULL) != 0)	
		cout<<"pthread_mutex_init err\n";
	if(pthread_cond_init(&_cond, NULL) != 0)	
		cout<<"pthread_conn_init err\n";
	
	int i;
	pthread_t pid;
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	stThreadTask task;
	//	create threads
	for(i=0;i<THREAD_NUM;i++)
	{
		if(pthread_create(&pid, NULL, threadpool_handle, this) != 0)
		{
			cout<<"i:"<<i<<" pthread_create err\n";
			break;
		}

		if(pthread_mutex_init(&mtx, NULL) != 0)	
		{
			cout<<"pthread_mutex_init err\n";
			break;
		}
		if(pthread_cond_init(&cond, NULL) != 0)	
		{
			cout<<"pthread_mutex_init err\n";
			break;
		}
		task.func = NULL;
		task.args = NULL;
		task.mtx  = mtx;
		task.cond = cond;
		task.bUsed = false;
		_mThread.insert(pair<pthread_t,stThreadTask>(pid, task));

		THREAD_TOTAL++;
		cout<<"init  create success pid:"<<pid<<endl;
	}
	cout<<"pool have ["<<THREAD_TOTAL<<"] threads can use\n";
}
int threadPool::thread_add(void (*function)(void*), void *args)
{
	int ret = -1;
	stThreadTask task;
	map<pthread_t, stThreadTask>::iterator itm;
	if(function == NULL)
		return -1;

	// _vThread full
	if(_usedThread >= THREAD_TOTAL)
	{
		;//cout<<"thread pool have full. max size is:"<<THREAD_TOTAL<<endl;
	}
	
	if(pthread_mutex_lock(&_mtx) != 0)
		return -1;

	do
	{
		for(itm=_mThread.begin();itm!=_mThread.end();itm++)
			if(itm->second.bUsed == false)
				break;
		if(itm == _mThread.end())
		{
		cout<<"thread pool have full. max size is:"<<THREAD_TOTAL<<endl;
			break;
		}

		pthread_mutex_lock(&(itm->second.mtx));
		itm->second.func = function;
		itm->second.args = args;
		itm->second.bUsed= true;
		cout<<"thread_add pid:"<<itm->first<<endl;
		_usedThread++;		

		if(pthread_cond_signal(&(itm->second.cond)) != 0)
		{
			cout<<"pthread_cond_signal err\n";
			break;
		}
		pthread_mutex_unlock(&(itm->second.mtx));
		ret = 0;

	}while(0);

	if(pthread_mutex_unlock(&_mtx) != 0)
		return -1;

	return ret;
}

int threadPool::thread_finish(pthread_t pid)
{
	map<pthread_t, stThreadTask>::iterator itm;
	pthread_mutex_lock(&_mtx);
	if((itm=_mThread.find(pid)) != _mThread.end())
	{
		if(_usedThread > 0)
			_usedThread--;	
	}
	pthread_mutex_unlock(&_mtx);
}
void func_cb(void *args)
{
	int i;
	for(i=0;i<5;i++)
	{
		cout<<pthread_self()<<" "<<(char*)args<<" func_cb running....."<<i<<endl;
	}

}

#if 0
int main()
{
	threadPool *tp = new threadPool(3);
	char str1[] = "hello111";
	char str2[] = "hello222";
	char str3[] = "hello333";
	sleep(1);
	tp->thread_add(func_cb, (void*)str1);
	tp->thread_add(func_cb, (void*)str2);
	tp->thread_add(func_cb, (void*)str3);
	tp->thread_add(func_cb, (void*)str3);
	sleep(2);
	tp->thread_add(func_cb, (void*)str2);
	sleep(18);
}
#endif


