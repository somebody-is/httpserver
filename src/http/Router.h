#ifndef __ROUTER_H__
#define __ROUTER_H__
#include<iostream>
#include<string>
#include"RequestHandler.h"

using namespace std;

class Router {
public:
    Router();
    void addRoute(const string path, shared_ptr<RequestHandler> handler);
    void routeRequest(const HttpRequest request, HttpResponse& response);

private:
    map<string, shared_ptr<RequestHandler>> routes;
};

#endif