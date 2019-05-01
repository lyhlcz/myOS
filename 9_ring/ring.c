#include <stdio.h>
#include <pthread.h>

#define N 10    //number of threads

//thread arg
typedef struct{
    int id;
} thdarg;

//semaphore var
typedef struct{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

void sema_init(sema_t* sema, int value){
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t* sema){
    pthread_mutex_lock(&sema->mutex);
    while(sema->value <= 0){
        pthread_cond_wait(&sema->cond, &sema->mutex);
    }

    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t* sema){
    pthread_mutex_lock(&sema->mutex);
    sema->value++;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

sema_t mutex_semas[N];

int x;

void* add_x(void* arg){
    int i = ((thdarg*)arg)->id;
    //printf("tid is %d\n", i);

    while(1){
        sema_wait(mutex_semas+i);
        x++;
        printf("T%d:\t%d\n", i+1, x);
        sema_signal(mutex_semas+((i+1)%N));
    }
}

int main(){
    pthread_t tids[N];
    thdarg args[N];
    int i;
    x = 0;

    //init
    sema_init(mutex_semas, 1);
    for(i=1; i<N; i++)
        sema_init(mutex_semas+i, 0);

    //thread create
    for(i=0; i<N; i++){
        args[i].id = i;
        pthread_create(tids+i, NULL, add_x, (void*)(args+i));
    }

    for(i=0; i<N; i++){
        pthread_join(tids[i], NULL);
    }

    return 0;
}