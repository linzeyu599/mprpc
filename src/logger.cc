#include "logger.h"
#include <time.h>
#include <iostream>

//获取日志的单例
Logger& Logger::GetInstance()//初始化
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    //启动专门的写日志线程
    std::thread writeLogTask([&](){
        for (;;)
        {
            //获取当前的日期，然后取日志信息，写入相应的日志文件当中 a+ //追加的方式，如果没有就创建
            time_t now = time(nullptr);//获取当前的时间，按秒算的，1970年到现在的
            tm *nowtm = localtime(&now);//返回tm结构的指针

            char file_name[128];//文件名
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);//构建文件，年-月-日-log.txt

            FILE *pf = fopen(file_name, "a+");//追加的方式打开
            if (pf == nullptr)//打开失败
            {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.Pop();//出队列

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s] ", 
                    nowtm->tm_hour, 
                    nowtm->tm_min, 
                    nowtm->tm_sec,
                    (m_loglevel == INFO ? "info" : "error"));
            msg.insert(0, time_buf);//时 分 秒
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    //设置分离线程，相当于一个守护线程，在后台专门去写日志
    writeLogTask.detach();
}

//设置日志级别 
void Logger::SetLogLevel(LogLevel level)
{
    m_loglevel = level;
}

//写日志，把日志信息写入lockqueue缓冲区当中，这是RPC请求的业务做的
void Logger::Log(std::string msg)
{
    m_lckQue.Push(msg);
}