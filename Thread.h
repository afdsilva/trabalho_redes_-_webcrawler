/*
 * Thread.h
 *
 *  Created on: 15/07/2013
 *      Author: andref
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <list>
#include <stdlib.h>
#include <pthread.h>

using namespace std;
class Thread {
private:
	pthread_t _id;
	// Prevent copying or assignment
	Thread(const Thread& arg);
	Thread& operator=(const Thread& rhs);
protected:
	bool started;
	void * arg;
	static void * exec(void *thr);
public:
	Thread();
	virtual ~Thread();
	unsigned int tid() const;
	void start(void *arg = NULL);
	void join();
	virtual void run() = 0;
};

class Lock {
protected:
	pthread_mutex_t mutex;
	// Prevent copying or assignment
	Lock(const Lock& arg);
	Lock& operator=(const Lock& rhs);
public:
	Lock();
	virtual ~Lock();
	void lock();
	void unlock();
};

class Condition : public Lock {
protected:
	pthread_cond_t cond;
	// Prevent copying or assignment
	Condition(const Condition& arg);
	Condition& operator=(const Condition& rhs);
public:
	Condition();
	virtual ~Condition();
	void wait();
	void notify();
};

#endif /* THREAD_H_ */
