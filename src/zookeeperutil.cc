#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

//全局的watcher观察器  zh表示zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type,
                   int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)//回调的消息类型  是和会话相关的消息类型
	{
		if (state == ZOO_CONNECTED_STATE)//zkclient和zkserver连接成功，才建立会话
		{
			sem_t *sem = (sem_t*)zoo_get_context(zh);//指定的句柄获取信号量
            sem_post(sem);//信号量资源加1
		}
	}
}

ZkClient::ZkClient() : m_zhandle(nullptr)
{
}

ZkClient::~ZkClient()//析构函数
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);//关闭句柄，释放资源,相当于MySQL_Conn
    }
}

//连接zkserver，2181端口
void ZkClient::Start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;
    
	/*
	zookeeper_mt：多线程版本
	zookeeper的API客户端程序提供了三个线程：
	API调用线程 也就是当前线程 也就是调用的线程 zookeeper_init
	网络I/O线程  zookeeper_init调用了pthread_create  poll专门连接网络
	watcher回调线程 当客户端接收到zkserver的响应，再次调用pthread_create，产生watcher回调线程
	*/
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);//global_watcher回调函数
	//返回的就是句柄，会话的创建是异步的，并不是返回成功了就是表示连接成功的，等回调函数真正接收到zkserver响应进行回调

    if (nullptr == m_zhandle)//表示发起的动作没有产生，传的参数有问题
    {
        std::cout << "zookeeper_init error!" << std::endl;
        exit(EXIT_FAILURE);//直接退出
    }

    sem_t sem;//创建信号量
    sem_init(&sem, 0, 0);//初始化为0
    zoo_set_context(m_zhandle, &sem);//创建句柄资源成功后，向句柄资源上设置上下文，添加额外的信息

    sem_wait(&sem);//等待，因为初始的信号量是0，阻塞着，等着zkserver响应（回调）。等加1。成功了才向下走。
    std::cout << "zookeeper_init success!" << std::endl;//global_watcher执行成功，连接成功了
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)//创建节点
{
    char path_buffer[128];
    int bufferlen = sizeof(path_buffer);
    int flag;

	//先判断path表示的znode节点是否存在，如果存在，就不再重复创建了
	flag = zoo_exists(m_zhandle, path, 0, nullptr);
	if (ZNONODE == flag)//表示path的znode节点不存在
	{
		//创建指定path的znode节点了
		flag = zoo_create(m_zhandle, path, data, datalen,
			&ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
		//句柄，路径，数据，数据长度，flag默认创建永久性
		if (flag == ZOK)//创建成功
		{
			std::cout << "znode create success... path:" << path << std::endl;
		}
		else//创建失败
		{
			std::cout << "flag:" << flag << std::endl;
			std::cout << "znode create error... path:" << path << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

//根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
	int bufferlen = sizeof(buffer);
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
	if (flag != ZOK)//获取失败
	{
		std::cout << "get znode error... path:" << path << std::endl;
		return "";
	}
	else
	{
		return buffer;
	}
}