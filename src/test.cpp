#include "./server/TcpServer.h"

#include <unistd.h>

/* 线程池中的线程，模拟处理业务 */
void *process(void *arg)
{
    printf("thread %ld working on task %d\n", pthread_self(), (int)(long)arg);
    sleep(10);
    return NULL;
}

int main()
{

    // ThreadPool pool(3,100,100);   /*创建线程池，池里最小3个线程，最大100，队列最大100*/
    // pool.set_log(DEBUG,FILE);
    // pool.threadpool_create();
    // int num[20], i;
    // for (i = 0; i < 20; i++) {
    //     num[i] = i;
    //     pool.threadpool_add_task(process, (void*)(long)num[i]);   /* 向线程池中添加任务 */
    // }
    // sleep(100);
    // pool.threadpool_destroy();

    TcpServer server(8080);
    server.set_log(LOG_INFO,LOG_CMD);
    server.init_socket();
    server.listening();
    while (1);
    // string message = "POST /index.html?id=5&uid=sf5456a=sdfg454&time=55 HTTP/1.1\r\n"
    //                  "user-agent: ApiPOST Runtime +https://www.apipost.cn\r\n"
    //                  "accept: */*\r\n"
    //                  "accept-encoding: gzip, deflate, br\r\n"
    //                  "connection: keep-alive\r\n"
    //                  "admin: admin\r\n"
    //                  "Host: 8.134.137.252:8080\r\n"
    //                  "Content-Type: multipart/form-data; boundary=--------------------------879968356421469967062833\r\n"
    //                  "Content-Length: 280\r\n"
    //                  "\r\n"
    //                  "----------------------------879968356421469967062833\r\n"
    //                  "Content-Disposition: form-data; name=\"username\"\r\n"
    //                  "\r\n"
    //                  "user\r\n"
    //                  "----------------------------879968356421469967062833\r\n"
    //                  "Content-Disposition: form-data; name=\"password\"\r\n"
    //                  "\r\n"
    //                  "123456\r\n"
    //                  "----------------------------879968356421469967062833--\r\n"
    //                  "\r\n";
    // HttpRequest request;
    // request.parse(message);
    //cout << "请求体：" << request.get_body() << endl;
    return 0;
}