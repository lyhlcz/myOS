#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N 20

struct thdarg{
    int* a;
    int n;
};

void* select_sort(void* args){
    if (!args){
        printf("select sort : error\n");
        return;
    }

    struct thdarg* targ = (struct thdarg*)args;
    int* a = targ->a;
    int n = targ->n;
    int* tmp = (int*)malloc(n);
    int i, j, t, max;

    while(n){
        max = *a;
        j = 0;
        for(i = 1; i < n; i++){
            if (max < *(a+i)){
                max = *(a+i);
                j = i;
            }
        }
        n--;
        *(a+j) = *(a+n);
        *(a+n) = max;
    }
}

void merge_sort(int* a, int la, int* b, int lb, int* c){
    if (!a || !b){
        printf("merge sort : error1\n");
        return;
    }
    int i = 0, j = 0, k = 0;
    int n = la;
    int m = lb;
    while(i<n && j<m){
        if (*(a+i) < *(b+j))
            *(c+k++) = *(a+i++);
        else
            *(c+k++) = *(b+j++);
    }
    if (i == n){
        while(j < m){
            *(c+k++) = *(b+j++);
        }
    }
    else if (j == m){
        while(i < n){
            *(c+k++) = *(a+i++);
        }
    }
    else
        printf("merge sort : error2\n");
}

void display(int* a){
    int n = N;
    int i;
    for(i = 0; i<n ;i++){
        printf("%d ", *(a+i));
    }
    printf("\n");
}

int main(){
    int a[N], c[N];
    int* b = a+N/2;
    int i;
    struct thdarg* args = (struct thdarg*)malloc(sizeof(struct thdarg));
    struct thdarg* args1 = (struct thdarg*)malloc(sizeof(struct thdarg));
    args->a = a;
    args->n = N/2;
    args1->a = b;
    args1->n = N/2;

    //create rand array
    srand((unsigned)time(NULL));
    for(i=0; i<N; i++)
        a[i] = rand()%101;

    //before sorting
    printf("before:\n");
    display(a);

    //select_sort half
    pthread_t ptid;
    pthread_create(&ptid, NULL, &select_sort, (void*)args1);

    //select_sort another half
    select_sort((void*)args);
    pthread_join(ptid, NULL);

    //after select sorting
    printf("half:\n");
    display(a);

    //merge_sort
    merge_sort(a, N/2, b, N/2, c);

    //after merge sorting
    printf("after:\n");
    display(c);
}