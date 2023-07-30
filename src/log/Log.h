#ifndef __LOG_H_
#define __LOG_H_ 1
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdarg.h>
#include <ctime>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <vector>

using namespace std;

enum level { LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_DEBUG};
enum dest { LOG_CMD, LOG_FILE};

class Logger
{
    
#define info(format, ...) do_log(LOG_INFO, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define debug(format, ...) do_log(LOG_DEBUG, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define warning(format, ...) do_log(LOG_WARNING, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define error(format, ...) do_log(LOG_ERROR, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)
  
private:
    
    int level;
    int dest;
    string filename;

    void print(string str);
    void printToCmd(string str);
    void printToFile(string str);

public:
static pthread_mutex_t out_lock;
    Logger();
    Logger(int level);
    Logger(int level, int dest);
    Logger(int level, int dest, string filename);
    void set_log(int level,int dest);
    void log(const char *func, int line, const char *level, const char *format,vector<string> data, int length);
    template <typename T>
    string argToString(T arg)
    {
        stringstream ss;
        ss << arg;
        return ss.str();
    }
    template <typename... Args>
    void do_log(int type, const char *function, int line, const char *format, Args... args)
    {
        string level;
        if (type == LOG_DEBUG)
            this->level >= LOG_DEBUG ? level = "DEBUG" : level = "NULL";
        else if (type == LOG_INFO)
            this->level >= LOG_INFO ? level = "INFO" : level = "NULL";
        else if (type == LOG_WARNING)
            this->level >= LOG_WARNING ? level = "WARNING" : level = "NULL";
        else if (type == LOG_ERROR)
            this->level >= LOG_ERROR ? level = "ERROR" : level = "NULL";
        if (level == "NULL")
            return;
        vector<string> data = {argToString(args)...};
        log(function, line, level.c_str(), format, data, data.size());
    }

};


#endif