#include <iostream>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include <vector>
#include "logger.h"

class FriendService : public fixbug::FiendServiceRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid)//返回好友的列表，本地方法
    {
        std::cout << "do GetFriendsList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("linyouhua");
        vec.push_back("lincanhui");
        vec.push_back("zhang san");
        return vec;
    }

    //重写基类方法,框架帮我们调用的
    void GetFriendsList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->userid();//获取用户的id号
        std::vector<std::string> friendsList = GetFriendsList(userid);//调用本地方法
        response->mutable_result()->set_errcode(0);//执行成功
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendsList)//遍历好友列表
        {
            std::string *p = response->add_friends();//添加
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char **argv)
{

    LOG_ERR("ddddd");
    LOG_INFO("ddddd");
    //调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    //provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    //启动一个rpc服务发布节点 Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}