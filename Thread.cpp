/*
 * Thread.cpp
 *
 *  Created on: 15/07/2013
 *      Author: andref
 */

#include "Thread.h"

using namespace std;

Thread::Thread() : started(false) {
	//cout << "Thread::Thread()" << endl;
}
Thread::~Thread() {
	//cout << "Thread::~Thread()" << endl;
}
unsigned int Thread::tid() const {
	return _id;
}

// Uses default argument: arg = NULL
void Thread::start(void *arg) {
	cout << "Thread::start()" << endl;
	int ret;
	if (!started) {
	started = true;
	this->arg = arg;
	/*
	 * Since pthread_create is a C library function, the 3rd
	 * argument is a global function that will be executed by
	 * the thread. In C++, we emulate the global function using
	 * the static member function that is called exec. The 4th
	 * argument is the actual argument passed to the function
	 * exec. Here we use this pointer, which is an instance of
	 * the Thread class.
	 */
	if ((ret = pthread_create(&_id, NULL, &Thread::exec, this)) != 0) {
			//cout << strerror(ret) << endl;
			throw "Error";
		}
	}
	sleep(2);
}
void Thread::join() {
	// Allow the thread to wait for the termination status
	pthread_join(_id, NULL);
}
// Function that is to be executed by the thread
void * Thread::exec(void *thr) {
	cout << "Thread::exec()" << endl;
	reinterpret_cast<Thread *> (thr)->run();
	return NULL;
}

Lock::Lock() {
	pthread_mutex_init(&mutex, NULL);
}
Lock::~Lock() {
	pthread_mutex_destroy(&mutex);
}
void Lock::lock() {
	pthread_mutex_lock(&mutex);
}
void Lock::unlock() {
	pthread_mutex_unlock(&mutex);
}

Condition::Condition() {
	pthread_cond_init(&cond, NULL);
}
Condition::~Condition() {
	pthread_cond_destroy(&cond);
}
void Condition::wait() {
	pthread_cond_wait(&cond, &mutex);
}
void Condition::notify() {
	pthread_cond_signal(&cond);
}
