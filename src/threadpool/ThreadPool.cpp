#include "ThreadPool.h"
ThreadPool::ThreadPool(int min_thr_num, int max_thr_num, int queue_max_size)
{
    this->min_thr_num = min_thr_num;
    this->max_thr_num = max_thr_num;
    this->busy_thr_num = 0;
    this->live_thr_num = min_thr_num; /* 活着的线程数 初值=最小线程数 */
    this->wait_exit_thr_num = 0;
    this->adjust_tid = 0;
    this->queue_max_size = queue_max_size; /* 最大任务队列数 */
    this->shutdown = false;                /* 不关闭线程池 */
    this->ptr = this;
    this->logger = new Logger(LOG_DEBUG);
    this->logger->info("初始化线程池");
}
void ThreadPool::threadpool_create()
{
    int i;
    do
    {
        if (pthread_mutex_init(&(this->lock), NULL) != 0 || pthread_mutex_init(&(this->thread_counter), NULL) != 0 || pthread_cond_init(&(this->queue_not_empty), NULL) != 0 || pthread_cond_init(&(this->queue_not_full), NULL) != 0)
        {
            this->logger->error("初始化线程锁或条件变量出错！");
            break;
        }
        pthread_t pid;
        for (i = 0; i < min_thr_num; i++)
        {
            pthread_create(&pid, NULL, threadpool_thread, (void *)this->ptr);
            this->threads.push_back(pid);
        }
        this->logger->info("成功创建{}个线程", min_thr_num);
        pthread_create(&this->adjust_tid, NULL, adjust_thread, (void *)this->ptr);
        this->logger->info("创建管理者线程：{}", this->adjust_tid);
        return;
    } while (0);
    this->logger->error("线程池创建失败");
}
void *ThreadPool::threadpool_thread(void *threadpool)
{
    ThreadPool *pool = (ThreadPool *)threadpool;
    threadpool_task_t task;
    while (true)
    {
        pthread_mutex_lock(&pool->lock);
        while (pool->task_queue.size() == 0 && !pool->shutdown)
        {
            pthread_cond_wait(&pool->queue_not_empty, &pool->lock);
            if (pool->wait_exit_thr_num > 0 && pool->live_thr_num > pool->min_thr_num)
            {
                pool->wait_exit_thr_num--;
                pool->live_thr_num--;
                pthread_mutex_unlock(&pool->lock);
                pthread_detach(pthread_self());
                pool->logger->debug("线程 pid={}，结束运行", pthread_self());
                pool->threads.erase(remove(pool->threads.begin(), pool->threads.end(), pthread_self()), pool->threads.end());
                pthread_exit(NULL);
            }
        }

        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->lock);
            pthread_detach(pthread_self());
            pool->logger->debug("线程 pid={}，结束运行", pthread_self());
            pool->threads.erase(remove(pool->threads.begin(), pool->threads.end(), pthread_self()), pool->threads.end());
            pthread_exit(NULL);
        }
        task.function = pool->task_queue.front().function;
        task.arg = pool->task_queue.front().arg;
        pool->task_queue.pop();

        pthread_cond_broadcast(&pool->queue_not_full);
        pthread_mutex_unlock(&pool->lock);

        /*执行任务*/
        pool->logger->debug("线程 pid={}，开始工作", pthread_self());
        pthread_mutex_lock(&pool->thread_counter);
        pool->busy_thr_num++;
        pthread_mutex_unlock(&pool->thread_counter);
        (*(task.function))(task.arg);
        //cout<<"执行任务\n";
        pthread_mutex_lock(&pool->thread_counter);
        pool->busy_thr_num--;
        pthread_mutex_unlock(&pool->thread_counter);
        pool->logger->debug("线程 pid={}，工作结束", pthread_self());
    }
    return NULL;
}

void *ThreadPool::adjust_thread(void *threadpool)
{
    
    ThreadPool *pool = (ThreadPool *)threadpool;
    int i;
    while (!pool->shutdown)
    {
        sleep(DEFAULT_TIME);
        pthread_mutex_lock(&pool->lock);
        int queue_size = pool->task_queue.size();
        int live_thr_num = pool->live_thr_num;
        pthread_mutex_unlock(&pool->lock);

        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num; /* 忙着的线程数 */
        pthread_mutex_unlock(&(pool->thread_counter));
        
        if (queue_size >= MAX_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num)
        {
            pool->logger->debug("任务队列超过阈值:[{}]，创建线程", queue_size);
            pthread_mutex_lock(&pool->lock);
            int add = 0;

            for (i = 0; add < DEFAULT_THREAD_ADD && pool->live_thr_num < pool->max_thr_num; i++)
            {
                pthread_t pid;
                pthread_create(&pid, NULL, threadpool_thread, (void *)pool);
                pool->threads.push_back(pid);
                add++;
                pool->live_thr_num++;
            }
            pthread_mutex_unlock(&(pool->lock));
        }

        if (live_thr_num - busy_thr_num > MAX_FREE_THREAD_NUM && live_thr_num > pool->min_thr_num)
        {
            pool->logger->debug("空闲线程超过阈值:[{}]，销毁线程", live_thr_num - busy_thr_num);
            pthread_mutex_lock(&pool->lock);
            pool->wait_exit_thr_num = DEFAULT_THREAD_DEL;
            pthread_mutex_unlock(&pool->lock);
            for (i = 0; i < DEFAULT_THREAD_DEL; i++)
            {
                /* 通知处在空闲状态的线程, 他们会自行终止*/
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }
    }
    return NULL;
}
int ThreadPool::threadpool_add_task(void *(*function)(void *arg), void *arg)
{
    pthread_mutex_lock(&this->lock);
    while (this->task_queue.size() > (size_t)this->queue_max_size && !this->shutdown)
    {
        pthread_cond_wait(&this->queue_not_full, &this->lock);
    }
    if (this->shutdown)
    {
        pthread_cond_broadcast(&this->queue_not_empty);
        pthread_mutex_unlock(&this->lock);
        return 0;
    }
    this->logger->debug("向线程池添加任务");
    threadpool_task_t task;
    task.function = function;
    task.arg = arg;
    this->task_queue.push(task);

    pthread_cond_signal(&this->queue_not_empty);
    pthread_mutex_unlock(&this->lock);

    return 0;
}
int ThreadPool::threadpool_destroy()
{
    int i;
    this->shutdown = true;

    /*先销毁管理线程*/
    //pthread_join(this->adjust_tid, NULL);
    this->logger->info("共{}个线程,销毁线程池中...",this->live_thr_num);
    for (i = 0; i < this->live_thr_num; i++)
    {
        /*通知所有的空闲线程*/
        pthread_cond_broadcast(&(this->queue_not_empty));
    }
    this->logger->info("销毁线程池完成");
    return 0;
}

void ThreadPool::set_log(int level,int dest)
{
    this->logger->set_log(level,dest);
}
