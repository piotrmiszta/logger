#include "utility.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define RED                 "\x1B[31m"
#define GREEN               "\x1B[32m"
#define PURPLE              "\x1B[35m"
#define BLUE                "\x1B[36m"
#define YELLOW              "\x1B[37m"
#define RESET               "\x1B[0m"


typedef struct LoggerStruct{
    char msg[1000];
    char file[50];
    char func[50];
    uint64_t line;
    LogerType type;
}LoggerStruct;

static thrd_t thread;
static mtx_t mutex;
static semaphore_t semFull;
static semaphore_t semEmpty;
static Queue* que;
static bool run;

static int logger_thread(void* arg);
static void log_print(FILE* file, LoggerStruct* to_print);
static inline void print_time(FILE* stream);

int logger_init(char* file_name) {
    FILE* file = fopen(file_name, "w");
    assert(file);
    mtx_init(&mutex, mtx_plain);
    semaphore_init(&semFull, 0);
    semaphore_init(&semEmpty, 10);
    que = queue_create(free);
    run = 1;
    thrd_create(&thread, logger_thread, file);
    return 0;
}

void logger_close(void) {
    run = 0;
    thrd_join(thread, NULL);
    semaphore_destroy(&semEmpty);
    semaphore_destroy(&semFull);
    mtx_destroy(&mutex);
    queue_destroy(que);
}

static int logger_thread(void* arg) {
    FILE* file = arg;
    while(run) {
        LoggerStruct* to_print;
        semaphore_wait(&semFull);
        if(!run)
            break;
        mtx_lock(&mutex);
        to_print = queue_pop(que);
        mtx_unlock(&mutex);
        semaphore_post(&semEmpty);
        log_print(file, to_print);
        free(to_print);
    }
    /* end logger */
    mtx_lock(&mutex);
    while(!queue_empty(que)) {
        LoggerStruct* to_print;
        to_print = queue_pop(que);
        log_print(file, to_print);
        free(to_print);
    }
    mtx_unlock(&mutex);
    fclose(file);
    return 0;
}
#define ___LOG_CONSOLE
void log_print(FILE* file, LoggerStruct* log) {
    fprintf(file, "[ ");
    print_time(file);
    fprintf(file, " ] ");

#ifdef ___LOG_CONSOLE
    fprintf(stdout, "[ ");
    print_time(stdout);
    fprintf(stdout, " ] ");
#endif

    switch(log->type) {
        case LOGGER_TYPE_DEBUG:
            fprintf(file, GREEN"[ DEBUG ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, GREEN"[ DEBUG ]"RESET);
            #endif
            break;
        case LOGGER_TYPE_INFO:
            fprintf(file, PURPLE"[ INFO ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, PURPLE"[ INFO ]"RESET);
            #endif
            break;
        case LOGGER_TYPE_WARNING:
            fprintf(file, BLUE"[ WARNING ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, BLUE"[ WARNING ]"RESET);
            #endif
            break;
        case LOGGER_TYPE_TRACE:
            fprintf(file, YELLOW"[ TRACE ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, YELLOW"[ TRACE ]"RESET);
            #endif
            break;
        case LOGGER_TYPE_ERROR:
            fprintf(file, RED"[ ERROR ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, RED"[ ERROR ]"RESET);
            #endif
            break;
        case LOGGER_TYPE_FATAL:
            fprintf(file, RED"[ FATAL ]"RESET);
            #ifdef ___LOG_CONSOLE
            fprintf(stdout, RED"[ FATAL ]"RESET);
            #endif
            break;
    } //switch
    fprintf(file, "%s:%lu - %s -- %s\n", log->file, log->line, log->func, log->msg);
    #ifdef ___LOG_CONSOLE
    fprintf(stdout, "%s:%lu - %s -- %s\n", log->file, log->line, log->func, log->msg);
    #endif
}


void logger_log__(LogerType type,
    const char* file,
    const char* func,
    size_t line,
    const char* fmt, ...) {

    LoggerStruct* log = calloc(1, sizeof(LoggerStruct));
    assert(log);
    va_list list;
    va_start(list, fmt);
    vsnprintf(log->msg, sizeof(log->msg), fmt, list);
    va_end(list);
    strcpy(log->file, file);
    strcpy(log->func, func);
    log->line = line;
    log->type = type;

    semaphore_wait(&semEmpty);
    mtx_lock(&mutex);
    queue_push(que, log);
    mtx_unlock(&mutex);
    semaphore_post(&semFull);
}

static inline void print_time(FILE* stream) {
    time_t result = time(NULL);
    char * buff = asctime(gmtime(&result));
    buff[strlen(buff) - 1] = '\0';
    fprintf(stream, "%s", buff);
}