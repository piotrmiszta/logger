#include "utility.h"
#include <threads.h>

#include <stdio.h>
#include <stddef.h>
int semaphore_init(semaphore_t* sem, int64_t value) {
    if(sem == NULL) {
        return -1;
    }
    mtx_init(&sem->mutex, mtx_plain);
    cnd_init(&sem->cond);
    sem->value = value;
    return 0;
}

int semaphore_destroy(semaphore_t* sem) {
    if(sem == NULL) {
        return -1;
    }
    mtx_destroy(&sem->mutex);
    cnd_destroy(&sem->cond);
    return 0;
}

int semaphore_wait(semaphore_t* sem) {
    if(sem == NULL) {
        return -1;
    }
    mtx_lock(&sem->mutex);
    while(sem->value == 0) {
        cnd_wait(&sem->cond, &sem->mutex);
    }
    sem->value --;
    mtx_unlock(&sem->mutex);
    return 0;

}

int semaphore_post(semaphore_t* sem) {
    if(sem == NULL) {
        return -1;
    }
    mtx_lock(&sem->mutex);
    sem->value++;
    cnd_signal(&sem->cond);
    mtx_unlock(&sem->mutex);
    return 0;
}
