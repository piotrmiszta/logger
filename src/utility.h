#ifndef UTILITY_H_
#define UTILITY_H_
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <threads.h>
typedef struct Queue Queue;

Queue* queue_create(void(*free_data)(void* data));
void queue_destroy(Queue* que);
void queue_push(Queue* restrict que, void* data);
void* queue_pop(Queue* restrict que);
bool queue_empty(Queue* restrict que);

typedef struct DoubleLinkedList DoubleLinkedList;
typedef DoubleLinkedList DLL;

typedef struct Queue Queue;

typedef enum LogerType{
    LOGGER_TYPE_DEBUG,
    LOGGER_TYPE_INFO,
    LOGGER_TYPE_WARNING,
    LOGGER_TYPE_ERROR,
    LOGGER_TYPE_FATAL,
    LOGGER_TYPE_TRACE,
}LogerType;

typedef struct LoggerParams {
    char filename[100];
    char author[100];
    char contact[100];
    float version;
}LoggerParams;

int logger_init(LoggerParams * params);

void logger_close(void);

void logger_log__(LogerType type,
    const thrd_t thread,
    const char* file,
    const char* func,
    size_t line,
    const char* fmt, ...);

#define logger_log_(type, ...)  \
    logger_log__(type, thrd_current() , __FILE__, __func__, __LINE__, __VA_ARGS__)

#define LOG_DEBUG(...)          logger_log_(LOGGER_TYPE_DEBUG, __VA_ARGS__)
#define LOG_ERROR(...)          logger_log_(LOGGER_TYPE_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)          logger_log_(LOGGER_TYPE_FATAL, __VA_ARGS__)
#define LOG_INFO(...)           logger_log_(LOGGER_TYPE_INFO, __VA_ARGS__)
#define LOG_WARNING(...)        logger_log_(LOGGER_TYPE_WARNING, __VA_ARGS__)
#define LOG_TRACE(...)          logger_log_(LOGGER_TYPE_TRACE, __VA_ARGS__)
#define LOG_THREAD_START(...)   logger_log_(LOGGER_TYPE_THREAD_START, __VA_ARGS__)
#define LOG_THREAD_END(...)     logger_log_(LOGGER_TYPE_THREAD_START, __VA_ARGS__)


typedef struct semaphore_t {
    volatile int64_t value;
    mtx_t mutex;
    cnd_t cond;
}semaphore_t;

int semaphore_init(semaphore_t* sem, int64_t value);

int semaphore_destroy(semaphore_t* sem);

int semaphore_wait(semaphore_t* sem);

int semaphore_post(semaphore_t* sem);


DoubleLinkedList* dll_create(void (*custom_free)(void* data),
                             int (*custom_cmp)(void* arg, void* arg2));

void dll_destroy(DoubleLinkedList* doubleLinkedList);

void dll_push_back(DoubleLinkedList* doubleLinkedList, void* data);

void dll_push_forward(DoubleLinkedList* doubleLinkedList, void* data);

void* dll_pop_back(DoubleLinkedList* doubleLinkedList);

void* dll_pop_forward(DoubleLinkedList* doubleLinkedList);

int dll_find(DoubleLinkedList* doubleLinkedList, void* data);

size_t dll_get_size(DoubleLinkedList* doubleLinkedList);

#endif


