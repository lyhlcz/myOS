#include <stdio.h>
#include <pthread.h>

char buffer1[4];
char buffer2[4];
int in1, out1, in2, out2;

//buffer1 method
int buffer1_is_empty(){
    return in1 == out1;
}

int buffer1_is_full(){
    return (in1 + 1) % 4 == out1;
}

char get_item1(){
    char item;
    item = buffer1[out1];
    out1 = (out1 + 1) % 4;
    return item;
}

void put_item1(char item){
    buffer1[in1] = item;
    in1 = (in1 + 1) % 4;
}

//buffer2 method
int buffer2_is_empty(){
    return in2 == out2;
}

int buffer2_is_full(){
    return (in2 + 1) % 4 == out2;
}

char get_item2(){
    char item;
    item = buffer2[out2];
    out2 = (out2 + 1) % 4;
    return item;
}

void put_item2(char item){
    buffer2[in2] = item;
    in2 = (in2 + 1) % 4;
}

//conditional var
/*
pthread_mutex_t mutex1, mutex2;
pthread_cond_t wait_empty_buffer1;
pthread_cond_t wait_empty_buffer2;
pthread_cond_t wait_full_buffer1;
pthread_cond_t wait_full_buffer2;
*/

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

sema_t mutex1_sema;
sema_t mutex2_sema;
sema_t empty_buffer1_sema;
sema_t empty_buffer2_sema;
sema_t full_buffer1_sema;
sema_t full_buffer2_sema;


void* produce(void* arg){
    int i;
    char item;

    for(i=0; i<8; i++){
        /*
        pthread_mutex_lock(&mutex1);
        while(buffer1_is_full()){
            //printf("pro wait\n");
            pthread_cond_wait(&wait_empty_buffer1, &mutex1);
        }*/
        sema_wait(&empty_buffer1_sema);
        sema_wait(&mutex1_sema);

        item = 'a' + i;
        put_item1(item);
        printf("produce item: %c\n", item);

        /*
        pthread_cond_signal(&wait_full_buffer1);
        pthread_mutex_unlock(&mutex1);
        */
        sema_signal(&mutex1_sema);
        sema_signal(&full_buffer1_sema);
    }
}

void* calculate(void* arg){
    int i;
    char item;

    for(i=0; i<8; i++){
        /*
        pthread_mutex_lock(&mutex1);
        pthread_mutex_lock(&mutex2);
        while(1){
            if(buffer1_is_empty())
                pthread_cond_wait(&wait_full_buffer1, &mutex1);
            else if(buffer2_is_full())
                pthread_cond_wait(&wait_empty_buffer2, &mutex2);
            else
                break;
        }*/
        sema_wait(&full_buffer1_sema);
        sema_wait(&empty_buffer2_sema);
        sema_wait(&mutex1_sema);
        sema_wait(&mutex2_sema);

        item = get_item1();
        item = item - 32;
        put_item2(item);
        printf("               calculate item: %c\n", item);

        /*
        pthread_cond_signal(&wait_empty_buffer1);
        pthread_cond_signal(&wait_full_buffer2);
        pthread_mutex_unlock(&mutex1);
        pthread_mutex_unlock(&mutex2);
        */
        sema_signal(&mutex2_sema);
        sema_signal(&mutex1_sema);
        sema_signal(&full_buffer2_sema);
        sema_signal(&empty_buffer1_sema);
    }
}

void* consume(void* arg){
    int i;
    char item;

    for(i=0; i<8; i++){
        /*
        pthread_mutex_lock(&mutex2);
        while(buffer2_is_empty()){
            //printf("con wait\n");
            pthread_cond_wait(&wait_full_buffer2, &mutex2);
        }*/
        sema_wait(&full_buffer2_sema);
        sema_wait(&mutex2_sema);

        item = get_item2();
        printf("                                consume item: %c\n", item);

        /*
        pthread_cond_signal(&wait_empty_buffer2);
        pthread_mutex_unlock(&mutex2);
        */
        sema_signal(&mutex2_sema);
        sema_signal(&empty_buffer2_sema);
    }
}

int main(){
    pthread_t producer_id, calculator_id, consumer_id;

    //init
    /*
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&wait_empty_buffer1, NULL);
    pthread_cond_init(&wait_empty_buffer2, NULL);
    pthread_cond_init(&wait_full_buffer1, NULL);
    pthread_cond_init(&wait_full_buffer2, NULL);
    */
    sema_init(&mutex1_sema, 1);
    sema_init(&mutex2_sema, 1);
    sema_init(&empty_buffer1_sema, 3);
    sema_init(&empty_buffer2_sema, 3);
    sema_init(&full_buffer1_sema, 0);
    sema_init(&full_buffer2_sema, 0);

    //thread create
    pthread_create(&producer_id, NULL, produce, NULL);
    pthread_create(&calculator_id, NULL, calculate, NULL);
    pthread_create(&consumer_id, NULL, consume, NULL);

    //thread syn
    pthread_join(producer_id, NULL);
    pthread_join(calculator_id, NULL);
    pthread_join(consumer_id, NULL);

    /*
    //destroy
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&wait_empty_buffer1);
    pthread_cond_destroy(&wait_empty_buffer2);
    pthread_cond_destroy(&wait_full_buffer1);
    pthread_cond_destroy(&wait_full_buffer2);
    */

    return 0;
}