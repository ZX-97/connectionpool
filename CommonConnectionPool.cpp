#include "CommonConnectionPool.h"
#include "public.h"


//线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool; //对于静态局部变量的初始化由系统自动lock 和 unlock;
	return &pool;
}
//加载日志文件
bool ConnectionPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not exist!");
		return false;
	}
	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find("=", 0);//未找到"=", 返回-1，
		if (idx == -1) { //无效配置项
			continue;
		}
		int endidx = str.find("\n", idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);
		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}
//构造连接池
ConnectionPool::ConnectionPool() {
	//加载配置项
	if (!loadConfigFile()) {
		return;
	}
	//创建初始连接数量
	for (int i = 0; i < _initSize; i++) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
	    _connectionCnt++;
	}

	//创建生产者线程
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();

}
//生产者函数
void ConnectionPool::produceConnectionTask() {
	for (; ;) {
		unique_lock<mutex> mylock(_queueMutex);
		while(!_connectionQue.empty()) {
			cv.wait(mylock);
		}
		if (_connectionCnt < _maxSize) {
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_connectionCnt++;
		}
		cv.notify_all();
	}

}
//消费者函数
shared_ptr<Connection> ConnectionPool::getconnection() {
	unique_lock<mutex> mylock(_queueMutex);
	while (_connectionQue.empty()) {//防止虚假唤醒
		if (cv_status::timeout == cv.wait_for(mylock, chrono::milliseconds(_connectionTimeout))) {
			LOG("获取空闲连接超时了...获取连接失败")
				return nullptr;
		}
	}
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection *pcon) {
			unique_lock<mutex> mylock(_queueMutex);
			pcon->refreshAliveTime();
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	cv.notify_all();
	return sp;
}

//扫描回收连接
void ConnectionPool::scannerConnectionTask() {
	for (; ;) {
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		unique_lock<mutex> mylock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection* p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000)) {
				_connectionQue.pop();
				_connectionCnt--;
				delete p;
			}
			else {
				break;
			}
		}
	}
}