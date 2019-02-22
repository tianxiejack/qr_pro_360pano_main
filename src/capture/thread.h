/*
 * thread.h
 *
 *  Created on: Dec 29, 2016
 *      Author: wang
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>

pthread_t start_thread(void *(*__start_routine) (void *), void * __arg);

#endif /* THREAD_H_ */
