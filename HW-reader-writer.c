#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "HW-reader-writer.h"

typedef struct rwlock_t {
    sem_t writelock;
    sem_t mutex;
    int AR;       // number of Active Readers
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    rw->AR = 0;
    Sem_init(&rw->mutex, 1); 
    Sem_init(&rw->writelock, 1); 
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    Sem_wait(&rw->mutex);
    rw->AR++;
    if (rw->AR == 1)
        Sem_wait(&rw->writelock);
    Sem_post(&rw->mutex);
}

void rwlock_release_readlock(rwlock_t *rw) {
    Sem_wait(&rw->mutex);
    rw->AR--;
    if (rw->AR == 0)
        Sem_post(&rw->writelock);
    Sem_post(&rw->mutex);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    Sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    Sem_post(&rw->writelock);
}


//
// Don't change the code below (just use it!) But fix it if bugs are found!
// 

int loops;
int DB = 0;

typedef struct arg_t{
    int thread_id;
    int job_type;         // 0: reader, 1: writer
    int arrival_delay;
    int running_time;
} arg_t;

void space(int s) {
    Sem_wait(&print_lock);
    printf("   %d\t",rwlock.AR);   // a[]의 정보를 반영해서 헤딩 라인을 출력
    int i;
    for (i = 0; i < s * 3; i++)
        printf("\t");
}

void space_end() {
    Sem_post(&print_lock);
}

void *worker(void *arg) {
    arg_t *args = (arg_t *)arg;
    int i;
    for (i = 0; i < args->arrival_delay; i++) {
        TICK;
        space(args->thread_id); printf("arrival delay %d of %d\n", i, args->arrival_delay); space_end();
    }
    if (args->job_type == 0) reader(arg);
    else if (args->job_type == 1) writer(arg);
    else {
        space(args->thread_id); printf("Unknown job %d\n",args->thread_id); space_end();
    }
    return NULL;
}
    
void *reader(void *arg) {
    arg_t *args = (arg_t *)arg;
    
    TICK;
    rwlock_acquire_readlock(&rwlock);
   // start reading
    int i;
    for (i = 0; i < args->running_time-1; i++) {
        TICK;
        space(args->thread_id); printf("reading %d of %d\n", i, args->running_time); space_end();
    }
    TICK;
    space(args->thread_id); printf("reading %d of %d, DB is %d\n", i, args->running_time, DB); space_end();
    // end reading
    TICK;
    rwlock_release_readlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    arg_t *args = (arg_t *)arg;

    TICK;
    rwlock_acquire_writelock(&rwlock);
   // start writing
    int i;
    for (i = 0; i < args->running_time-1; i++) {
        TICK;
        space(args->thread_id); printf("writing %d of %d\n", i, args->running_time); space_end();
    }
    TICK;
    DB++;
    space(args->thread_id);
    printf("writing %d of %d, DB is %d\n", i, args->running_time, DB);
    space_end();
    // end writing 
    TICK;
    rwlock_release_writelock(&rwlock);

    return NULL;
}


int main(int argc, char *argv[]) {
    
    // command line argument로 공급 받거나  
    // 예: -n 6 -a 0:0:5,0:1:8,1:3:4,0:5:7,1:6:2,0:7:4    또는   -n 6 -a r:0:5,r:1:8,w:3:4,r:5:7,w:6:2,r:7:4
    // 아래 코드에서 for-loop을 풀고 배열 a에 직접 쓰는 방법으로 worker 세트를 구성한다.
    
    if(argc != 2) printf("format : ./HW-reader_writer <filename>");
    int num_workers;
    pthread_t p[MAX_WORKERS];
    arg_t a[MAX_WORKERS];
    
    rwlock_init(&rwlock);
    Sem_init(&print_lock, 1);

    char* fil = argv[1];
    FILE* fp = fopen(fil,"r");
    char buffer[1000] = {0,};
    fgets(buffer, 100, fp);
    num_workers = (int)(buffer[0]-'0');
    printf("%d\n",num_workers);

    for (int i = 0; i < num_workers; i++) {
        fgets(buffer, 100, fp);
        a[i].thread_id = i;
        a[i].job_type = (int)(buffer[0]-'0'); // 0: reader, 1: writer
        a[i].arrival_delay = (int)(buffer[2]-'0');
        a[i].running_time = (int)(buffer[4]-'0');
        printf("%d %d %d\n", a[i].job_type, a[i].arrival_delay,a[i].running_time);

    }
    
    printf("begin\n");   int readerCnt=0,writerCnt=0;
    printf("   AR  ");   // a[]의 정보를 반영해서 헤딩 라인을 출력
    for(int i = 0;i<num_workers;i++){
        if(a[i].job_type == 0)
            printf("\treader %d\t",readerCnt++);
        else
            printf("\twriter %d\t",writerCnt++);
    }
    printf("\n -------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n");

    for (int i = 0; i < num_workers; i++){
        Pthread_create(&p[i], NULL, worker, &a[i]);
    }
    
    for (int i = 0; i < num_workers; i++){
        Pthread_join(p[i], NULL);
    }

    printf("end: DB %d\n", DB);

    return 0;
}


