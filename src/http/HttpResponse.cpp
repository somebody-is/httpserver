#include "HttpResponse.h"

HttpResponse::HttpResponse()
{
    this->status_code = 200;
    this->title = "OK";
    this->headers["Content-Type"] = "text/plain; charset=utf-8";
}

void HttpResponse::set_status_code(int code, string title)
{
    this->status_code = code;
    this->title = title;
}
void HttpResponse::add_header(string key, string value)
{
    headers[key] = value;
}
void HttpResponse::set_content_type(string content_type)
{
    headers["Content-Type"] = content_type;
}
void HttpResponse::set_body(string body)
{
    this->body = body;
}

string HttpResponse::get_response_str()
{
    stringstream ss;
    ss << "HTTP/1.1 " << status_code << " " << title << "\r\n";
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        ss << it->first<<": "<<it->second<<"\r\n";
    }
    ss<<"\r\n";
    ss<<body;
    ss<<"\n";
    return ss.str();
}

void HttpResponse::send_response(int fd)
{
    // const char *content = get_response_str().c_str();
    // cout<<"返回：\n";
    // cout<<content<<endl;
    // send(fd, content, strlen(content), 0);
    string content = get_response_str();
    send(fd, content.c_str(), content.size(), 0);
}