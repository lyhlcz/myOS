#include <stdio.h>
#include <pthread.h>

#define N 1000

void* cal(void* arg){
    double x = *(double*)arg;
    int c = N/2;
    double sign = 1.0;
    double result = 0.0;
    while(c--){
        result +=  sign/x;
        x += 2.0;
        sign = -sign;
    }

    *(double*)arg = result;
}

int main(){
    pthread_t ptid;
    double p_result = N - 1;
    pthread_create(&ptid, NULL, &cal, (void*)&p_result);

    double f_result = 1.0;
    cal((void*)&f_result);
    pthread_join(ptid, NULL);

    double result = f_result + p_result;
    f_result *= 4.0;
    p_result *= 4.0;
    result *= 4.0;
    printf("master :%lf\n", f_result);
    printf("worker :%lf\n", p_result);
    printf("total result is :%lf\n", result);
}