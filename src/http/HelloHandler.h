#ifndef __HELLOHANDLER_H__
#define __HELLOHANDLER_H__
#include <iostream>
#include <map>
#include "RequestHandler.h"

class HelloHandler : public RequestHandler
{
public:
    HelloHandler();
    virtual bool canHandle(const HttpRequest request) override;
    virtual void handleRequest(const HttpRequest request, HttpResponse &response) override;
};

#endif