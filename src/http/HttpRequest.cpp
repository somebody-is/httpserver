#include "HttpRequest.h"

void HttpRequest::parse(string message)
{
    // 提取请求方法
    regex methodRegex(R"((\w+)\s)");
    smatch methodMatch;
    regex_search(message, methodMatch, methodRegex);
    this->method = methodMatch[1];

    // 提取请求路径
    regex pathRegex(R"(\s(\S+)\s)");
    smatch pathMatch;
    regex_search(message, pathMatch, pathRegex);
    char str[pathMatch[1].str().size()];
    decode_str(str,pathMatch[1].str().c_str());
    this->url.assign(str);

    // 提取query
    regex queryRegex("(\\w+)=([^&]+)");
    smatch queryMatch;
    regex_search(this->url, queryMatch, queryRegex);
    std::sregex_iterator it(this->url.begin(), this->url.end(), queryRegex);
    std::sregex_iterator end;
    while (it != end) {
        std::smatch match = *it;
        querys[match[1]] = match[2];
        ++it;
    }

    // 提取请求头信息
    regex headerRegex(R"((.+): (.+)\r\n)");
    smatch headerMatch;

    auto headerBegin = sregex_iterator(message.begin(), message.end(), headerRegex);
    auto headerEnd = sregex_iterator();

    for (sregex_iterator iter = headerBegin; iter != headerEnd; ++iter) {
        smatch match = *iter;
        string key = match[1];
        string value = match[2];
        headers[key] = value;
    }

    // 提取请求体
    regex bodyRegex(R"(\r\n\r\n([\s\S]*))");
    smatch bodyMatch;
    regex_search(message, bodyMatch, bodyRegex);
    this->body = bodyMatch[1];
}
string HttpRequest::get_method()const
{
    return this->method;
}
string HttpRequest::get_url() const
{
    return this->url;
}
map<string, string> HttpRequest::get_headers()const
{
    return headers;
}
map<string, string> HttpRequest::get_querys()const
{
    return querys;
}
string HttpRequest::get_header(string header)const
{
    if(headers.count(header)>0)
        return headers.at(header);
    return nullptr;
}
string HttpRequest::get_body()const
{
    return body;
}

// 16进制数转化为10进制
int HttpRequest::hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}

/*
 *  这里的内容是处理%20之类的东西！是"解码"过程。
 *  %20 URL编码中的‘ ’(space)
 *  %21 '!' %22 '"' %23 '#' %24 '$'
 *  %25 '%' %26 '&' %27 ''' %28 '('......
 *  相关知识html中的‘ ’(space)是&nbsp
 */
void HttpRequest::encode_str(char* to, int tosize, const char* from)
{
    int tolen;

    for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from) {    
        if (isalnum(*from) || strchr("/_.-~", *from) != (char*)0) {      
            *to = *from;
            ++to;
            ++tolen;
        } else {
            sprintf(to, "%%%02x", (int) *from & 0xff);
            to += 3;
            tolen += 3;
        }
    }
    *to = '\0';
}

void HttpRequest::decode_str(char *to,const char *from)
{
    for ( ; *from != '\0'; ++to, ++from  ) {     
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {       
            *to = hexit(from[1])*16 + hexit(from[2]);
            from += 2;                      
        } else {
            *to = *from;
        }
    }
    *to = '\0';
}