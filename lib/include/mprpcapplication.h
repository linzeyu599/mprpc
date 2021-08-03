#pragma once
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的基础类，负责框架的一些初始化操作
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);//初始化 
    static MprpcApplication& GetInstance();//单例模式
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;
    MprpcApplication(){}//构造函数 
    MprpcApplication(const MprpcApplication&) = delete;//删除拷贝相关的 
    MprpcApplication(MprpcApplication&&) = delete;//删除拷贝相关的 
};