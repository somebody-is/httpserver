#ifndef __HTTPREQUEST_H__
#define __HTTPREQUEST_H__
#include <iostream>
#include <string>
#include <map>
#include <regex>

using namespace std;

class HttpRequest
{
private:
    string method;
    string url;
    map<string,string>headers;
    map<string,string>querys;
    string body;
    
public:
    HttpRequest(){};
    void parse(string message);
    string get_method()const;
    string get_url() const;
    map<string,string> get_headers()const;
    string get_header(string header)const;
    map<string,string> get_querys()const;
    string get_query(string header)const;
    string get_body()const;

    int hexit(char c);
    void encode_str(char* to, int tosize, const char* from);
    void decode_str(char *to,const char *from);
};

#endif