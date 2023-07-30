#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_
#include <iostream>
#include <pthread.h>
#include <queue>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include "../log/Log.h"

#define DEFAULT_TIME 10        /*10s检测一次*/
#define MAX_WAIT_TASK_NUM 10   /*最大等待任务数，超过此值则添加新的线程到线程池*/
#define MAX_FREE_THREAD_NUM 10 /*最大空闲线程数，超过此值则销毁部分线程*/
#define DEFAULT_THREAD_ADD 10  /*每次创建线程的个数*/
#define DEFAULT_THREAD_DEL 5  /*每次销毁线程的个数*/

using namespace std;

typedef struct
{
    void *(*function)(void *); /* 函数指针，回调函数 */
    void *arg;                 /* 上面函数的参数 */
} threadpool_task_t;           /* 各子线程任务结构体 */

class ThreadPool
{
private:
    Logger *logger;
    ThreadPool *ptr;
    pthread_mutex_t lock;                /* 用于锁住本结构体 */
    pthread_mutex_t thread_counter;      /* 记录忙状态线程个数de琐 -- busy_thr_num */
    pthread_cond_t queue_not_full;       /* 当任务队列满时，添加任务的线程阻塞，等待此条件变量 */
    pthread_cond_t queue_not_empty;      /* 任务队列里不为空时，通知等待任务的线程 */
    vector<pthread_t> threads;           /* 存放线程池中每个线程的tid。数组 */
    pthread_t adjust_tid;                /* 存管理线程tid */
    queue<threadpool_task_t> task_queue; /* 任务队列(数组首地址) */
    int min_thr_num;                     /* 线程池最小线程数 */
    int max_thr_num;                     /* 线程池最大线程数 */
    int live_thr_num;                    /* 当前存活线程个数 */
    int busy_thr_num;                    /* 忙状态线程个数 */
    int wait_exit_thr_num;               /* 要销毁的线程个数 */
    int queue_max_size;                  /* task_queue队列可容纳任务数上限 */
    int shutdown;                        /* 标志位，线程池使用状态，true或false */
    static void *threadpool_thread(void *threadpool);
    static void *adjust_thread(void *threadpool);

public:
    ThreadPool(int min_thr_num, int max_thr_num, int queue_max_size);
    void threadpool_create();
    int threadpool_add_task(void *(*function)(void *arg), void *arg);
    int threadpool_destroy();
    void set_log(int level,int dest);
};

#endif