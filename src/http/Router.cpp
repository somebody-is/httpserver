#include "Router.h"
#include "HelloHandler.h"

Router::Router()
{
    shared_ptr<RequestHandler> helloHandler = make_shared<HelloHandler>();


    routes["/hello/"] = helloHandler;
}
void Router::addRoute(const string path, shared_ptr<RequestHandler> handler)
{
    routes[path] = handler; // 将路径和对应的RequestHandler对象添加到映射表中
}

void Router::routeRequest(const HttpRequest request, HttpResponse &response)
{
    for (const auto &route : routes)
    {
        if (route.second->canHandle(request))
        {                                                   // 查找能处理请求的RequestHandler
            route.second->handleRequest(request, response); // 处理请求
            return;
        }
    }

    // 没有找到能处理请求的RequestHandler，返回404错误
    response.set_status_code(404, "404 Not Found");
    response.set_body("404 Not Found");
}