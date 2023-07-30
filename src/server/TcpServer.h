#ifndef __TCPSERVER_H_
#define __TCPSERVER_H_
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <list>
#include "../threadpool/ThreadPool.h"
#include "../http/HttpRequest.h"
#include "../log/Log.h"
#include "../http/Router.h"

using namespace std;

class TcpServer
{
#define OPEN_MAX 1024
#define MAX_EVENTS 100
    /*描述就绪文件描述符的相关信息*/
    struct myevent_s
    {
        int fd;     // 要监听的文件描述符
        int events; // 对应的监听事件，EPOLLIN和EPLLOUT
        void *arg;  // 指向自己结构体指针
        // void (*call_back)(void *arg); // 回调函数
        TcpServer *server;
        int status; // 是否在监听:1->在红黑树上(监听), 0->不在(不监听)
        char buf[BUFSIZ];
        int len;
        long last_active; // 记录每次加入红黑树 g_efd 的时间值
    };

private:
    Logger *logger;
    int port;
    int server_fd;
    int epoll_fd;
    struct sockaddr_in server_addr;
    list<myevent_s> myevents;
    TcpServer *ptr;
    ThreadPool *pool;

    void event_add(int fd, void *arg);
    static void *accept_conn(void *arg);
    static void *recv_data(void *arg);

public:
    TcpServer(int port)
    {
        this->port = port;
        logger = new Logger(LOG_DEBUG);
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        this->ptr = this;
        this->pool = new ThreadPool(5, 100, 100);
    }
    void init_socket();
    void listening();
    void set_log(int level, int dest);
};

#endif