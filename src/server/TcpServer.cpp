#include "TcpServer.h"

void TcpServer::init_socket()
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1)
    {
        logger->error("创建socket失败");
        exit(0);
    }
    int reuse = 1;
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    int ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        logger->error("绑定ip端口失败");
        exit(0);
    }
    listen(server_fd, 20);
    char server_ip[20];

    epoll_fd = epoll_create(OPEN_MAX);
    event_add(server_fd, NULL);

    
    this->pool->threadpool_create();

    logger->info("服务器启动成功, ip:[{}], 端口:[{}]", inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, sizeof(server_ip)), ntohs(server_addr.sin_port));
}

void TcpServer::event_add(int fd, void *arg)
{
    struct epoll_event event;
    struct myevent_s myevent;

    event.events = myevent.events = EPOLLIN | EPOLLET;
    myevent.server = this->ptr;
    myevent.arg = arg;
    myevent.last_active = time(NULL);
    myevent.fd = fd;
    myevents.push_back(myevent);
    struct myevent_s *tmp;
    tmp = &myevents.back();
    event.data.ptr = tmp;

    fcntl(myevent.fd, F_SETFL, fcntl(myevent.fd, F_GETFL) | O_NONBLOCK);
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tmp->fd, &event);
    if (ret == -1)
    {
        logger->error("添加监听事件失败,监听fd:[{}]", tmp->fd);
    }
}

void *TcpServer::accept_conn(void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(ev->server->server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1)
    {
        ev->server->logger->error("客户端连接失败");
    }
    char client_ip[20];
    ev->server->logger->info("连接到客户端fd:[{}],ip:[{}],端口:[{}]", client_fd, inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs(client_addr.sin_port));
    fcntl(client_fd, F_SETFL, fcntl(client_fd, F_GETFL) | O_NONBLOCK);
    ev->server->event_add(client_fd, NULL);
    return NULL;
}

void *TcpServer::recv_data(void *arg)
{
    struct myevent_s *ev = (struct myevent_s *)arg;
    char buf[BUFSIZ];
    int bytes_read;
    string message;
    while ((bytes_read = read(ev->fd, buf, sizeof(buf))) > 0)
    {
        message.append(buf);
        memset(buf, 0, sizeof(buf));
    }
    if (bytes_read == 0)
    {
        ev->server->logger->debug("客户端:fd=[{}]断开连接", ev->fd);
        close(ev->fd);
    }
    else
    {
        // ev->server->logger->debug("收到客户端:fd=[{}]消息,请求体:[{}]",ev->fd,message);
        // ev->server->logger->debug("收到客户端:fd=[{}]消息,请求体:[{}]",ev->fd,message);
        HttpRequest request;
        request.parse(message);
        ev->server->logger->info("请求方法[{}],请求路径[{}],请求体:[{}]", request.get_method(), request.get_url(), request.get_body());
        HttpResponse response;
        Router router;
        router.routeRequest(request,response);
        response.send_response(ev->fd);
        
        close(ev->fd);
    }
    return NULL;
}

void TcpServer::listening()
{
    int i;
    while (1)
    {
        struct epoll_event events[MAX_EVENTS];
        /*    long now = time(NULL);
            for(i=0; i < 100; i++, checkpos++)
            {
                if(checkpos == MAX_EVENTS);
                    checkpos = 0;
                if(g_events[checkpos].status != 1)
                    continue;
                long duration = now -g_events[checkpos].last_active;
                if(duration >= 60)
                {
                    close(g_events[checkpos].fd);
                    printf("[fd=%d] timeout\n", g_events[checkpos].fd);
                    eventdel(g_efd, &g_events[checkpos]);
                }
            } */
        // 调用eppoll_wait等待接入的客户端事件,epoll_wait传出的是满足监听条件的那些fd的struct epoll_event类型
        int nfd = epoll_wait(epoll_fd, events, MAX_EVENTS + 1, -1);
        if (nfd < 0)
        {
            logger->error("epoll_wait 错误");
            exit(-1);
        }
        for (i = 0; i < nfd; i++)
        {
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;
            // 如果监听的是读事件，并返回的是读事件
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
            {
                if (ev->fd == server_fd)
                {
                    pool->threadpool_add_task(accept_conn, (void *)ev);
                }
                else
                {
                    pool->threadpool_add_task(recv_data, (void *)ev);
                    // ev->server->recv_data(ev);
                }
                // threadpool_add(thp, process, (void*)ev);
            }
            // 如果监听的是写事件，并返回的是写事件
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                if (ev->fd == server_fd)
                {
                    ev->server->accept_conn(ev);
                }
                else
                {
                    ev->server->recv_data(ev);
                }
                // threadpool_add(thp, process, (void*)ev);
            }
        }
    }
}

void TcpServer::set_log(int level, int dest)
{
    
    this->logger->set_log(level, dest);
    this->pool->set_log(level, dest);
}