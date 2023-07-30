#include "HelloHandler.h"

HelloHandler::HelloHandler()
{
    path = "/hello/";
}

void HelloHandler::handleRequest(const HttpRequest request, HttpResponse &response)
{
    stringstream body;
    body<<"\n请求方法："<<request.get_method()<<endl;
    body<<"\n请求路径："<<request.get_url()<<endl;
    body<<"\n请求参数：\n";
    map<string,string>querys = request.get_querys();
    map<string,string>::iterator querys_it;
	for(querys_it=querys.begin();querys_it!=querys.end();querys_it++){
		body<<querys_it->first<<": "<<querys_it->second<<endl;
	}
    body<<"\n请求头：\n";
    map<string,string>headers = request.get_headers();
    map<string,string>::iterator headers_it;
	for(headers_it=headers.begin();headers_it!=headers.end();headers_it++){
		body<<headers_it->first<<": "<<headers_it->second<<endl;
	}

    body<<"\n请求体：\n"<<request.get_body()<<endl;
    response.set_body(body.str());
}

bool HelloHandler::canHandle(const HttpRequest request)
{
    if (path.compare(request.get_url().c_str()) <= 0)
        return true;
    return false;
}