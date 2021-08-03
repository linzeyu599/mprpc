#pragma once
#include "lockqueue.h"
#include <string>

//定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
//可变参，提供给用户更轻松的使用logger 
//snprintf， 缓冲区，缓冲区的长度，写的格式化字符串， ##__VA_ARGS__。
//代表了可变参的参数列表，填到缓冲区当中，然后 logger.Log(c)
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

#define LOG_ERR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0) \

//定义日志的级别
enum LogLevel
{
    INFO, //普通信息
    ERROR,//错误信息
};

//Mprpc框架提供的日志系统
class Logger
{
public:
    //获取日志的单例
    static Logger& GetInstance();
    //设置日志级别 
    void SetLogLevel(LogLevel level);
    //写日志
    void Log(std::string msg);
private:
    int m_loglevel;//记录日志级别
    LockQueue<std::string>  m_lckQue;//日志缓冲队列

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};