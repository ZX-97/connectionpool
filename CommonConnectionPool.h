#pragma once
#include <string>
using namespace std;
#include <queue>
#include "Connection.h"
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <memory>
#include <functional>


/*
实现连接池功能模块
*/


class ConnectionPool {
public:
	static ConnectionPool* getConnectionPool();

	shared_ptr<Connection> getconnection();//外部端口从连接池获取连接
private:
	ConnectionPool();

	bool loadConfigFile();//加载配置文件

	void produceConnectionTask();//生产者线程入口
	
	void scannerConnectionTask();//扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收

	string _ip;//mysql ip地址
	unsigned short _port;//mysql 端口号 3306
	string _username;//mysql 登录用户名
	string _password;//MySQL 登录密码
	string _dbname; // 连接数据库名称
	int _initSize;//连接池初始连接量
	int _maxSize;//连接池最大连接量
	int _maxIdleTime;//连接池最大空闲时间
	int _connectionTimeout;//连接池获取连接的超时时间

	queue<Connection*> _connectionQue;//存储MySQL连接的队列
	mutex _queueMutex; //维护连接队列的线程安全互斥锁
	atomic<int> _connectionCnt;// 记录连接所创建的connection连接的总数量 
	condition_variable cv;//用于通信
};