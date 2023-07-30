#ifndef __REQUESTHANDLER_H__
#define __REQUESTHANDLER_H__
#include <iostream>
#include "HttpRequest.h"
#include "HttpResponse.h"
class RequestHandler
{
protected:
    string path;

public:
    virtual bool canHandle(const HttpRequest request) = 0;
    virtual void handleRequest(const HttpRequest request, HttpResponse &response) = 0;
};

#endif