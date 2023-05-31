#ifndef __HW_reader_writer_h__
#define __HW_reader_writer_h__

#include "common.h"
#include "common_threads.h"

#ifdef linux
#include <semaphore.h>
#elif __APPLE__
#include "zemaphore.h"
#endif

#define MAX_WORKERS 10
#define TICK everyT()    // 1/100초 단위로 하고 싶으면 usleep(10000)
#define TAB 10

typedef struct rwlock_t rwlock_t;
typedef struct arg_t arg_t;

Zem_t print_lock;
rwlock_t rwlock;

void rwlock_init(struct rwlock_t *rw);
void rwlock_acquire_readlock(struct rwlock_t *rw);
void rwlock_release_readlock(struct rwlock_t *rw);
void rwlock_acquire_writelock(struct rwlock_t *rw);
void rwlock_release_writelock(struct rwlock_t *rw);

void everyT();
void space(int s);
void space_end();

void *worker(void *arg);
void *reader(void *arg);
void *writer(void *arg);

#endif //__HW_reader_writer_h__