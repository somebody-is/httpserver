#include "Log.h"
pthread_mutex_t Logger::out_lock = PTHREAD_MUTEX_INITIALIZER;
Logger::Logger()
{
    this->level = LOG_INFO;
    this->dest = LOG_CMD;
    this->filename = "out.log";
}
Logger::Logger(int level)
{
    this->dest = LOG_CMD;
    if (level < 1 || level > 4)
        this->level = LOG_INFO;
    else
        this->level = level;
    this->filename = "out.log";
}
Logger::Logger(int level, int dest)
{
    if (level < 1 || level > 4)
        this->level = LOG_INFO;
    else
        this->level = level;
    if (dest < 1 || dest > 2)
        this->dest = LOG_CMD;
    else
        this->dest = dest;
    this->filename = "out.log";
}
Logger::Logger(int level, int dest, string filename)
{
    if (level < 1 || level > 4)
        this->level = LOG_INFO;
    else
        this->level = level;
    if (dest < 1 || dest > 2)
        this->dest = LOG_CMD;
    else
        this->dest = dest;
    this->filename = filename;
}

void Logger::set_log(int level,int dest)
{
    this->level = level;
    this->dest = dest;
}

void Logger::log(const char *func, int line, const char *level, const char *format,vector<string> data, int length)
{
    stringstream buf;
    /*打印时间*/
    buf << "[" << level << "]";
    time_t now = time(0);
    tm *ltm = localtime(&now);
    buf << " [" << 1900 + ltm->tm_year << "-" << ltm->tm_mon << "-" << ltm->tm_mday << " " << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "]";
    /*打印函数信息*/
    buf << " [" << func << " : " << line << "] ";
    /*打印信息*/
    int index = 0;
    while (*format != '\0' && index < length)
    {
        if (*format == '{' && *(format + 1) == '}')
        {
            buf << data[index++];
            format++;
        }
        else
        {
            buf << *format;
        }
        format++;
    }
    if(*format != '\0')
        buf << format;

    print(buf.str());
}
void Logger::print(string str)
{
    pthread_mutex_lock(&out_lock);
    if (this->dest == LOG_CMD)
        this->printToCmd(str);
    else if (dest == LOG_FILE)
        printToFile(str);
    else{
        cout<<"[ERROR]\n";
    }
    pthread_mutex_unlock(&out_lock);
}
void Logger::printToCmd(string str)
{
    cout << str << endl;
    
}
void Logger::printToFile(string str)
{
    ofstream file;
    file.open(filename.c_str(), ios::out | ios::app);
    file << str<<endl;
}