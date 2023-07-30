#ifndef __HTTPREPONSE_H__
#define __HTTPREPONSE_H__
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <sys/socket.h>
#include <string.h>

using namespace std;

class HttpResponse
{
private:
    int status_code;
    string title;
    map<string, string> headers;
    string body;

public:
    HttpResponse();
    void set_status_code(int code,string title);
    void add_header(string key, string value);
    void set_content_type(string content_type);
    void set_body(string body);
    string get_response_str();
    void send_response(int fd);
};

#endif