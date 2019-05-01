#include <stdio.h>
#include <pthread.h>

#define N 5 //number of thread
#define M 10000  //task count of each thread

void* cal(void* arg){
    double x = *(double*)arg;
    int c = M;
    double result = 0.0;
    double sign = x<0?-1.0:1.0;
    while(c--){
        result += sign/x;
        x += 2.0;
        sign = -sign;
    }

    *(double*)arg = result * 4;
}

int main(){
    double result_array[N];
    pthread_t ptids[N];
    int i;

    //create threads
    double* p;
    for(i=0; i<N; i++){
        p = result_array + i;
        *p = 2*M*i+1;
        if((i*M)%2)
            *p = -*p;
        pthread_create(ptids+i, NULL, &cal, (void*)p);
    }

    //get results
    printf("result array is:\n");
    for(i=0; i<N; i++){
        pthread_join(ptids[i], NULL);
        printf("%lf ", result_array[i]);
    }
    printf("\n");

    //sum result
    double result = 0;
    for(i=0; i<N; i++){
        result += result_array[i];
    }
    printf("total result is :%lf\n", result);
}