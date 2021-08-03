#pragma once//防止头文件被重复包含 

#include<unordered_map>//键值对，方便读取这个key值项 ，配置文件不会一直改变 
#include<string>

//用于存放rpcserverip  rpcserverport  zookeeperip zookeeperport
//框架读取配置文件类
class MprpcConfig
{
public:
    //负责解析加载配置文件
    void LoadConfigFile(const char *config_file);
    //查询配置项信息
    std::string Load(const std::string &key);//返回key所对应的值-字符串 
private:
    std::unordered_map<std::string, std::string> m_configMap;
    //去掉字符串前后的空格
    void Trim(std::string &src_buf);
};