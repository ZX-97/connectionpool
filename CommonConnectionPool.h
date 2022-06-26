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
ʵ�����ӳع���ģ��
*/


class ConnectionPool {
public:
	static ConnectionPool* getConnectionPool();

	shared_ptr<Connection> getconnection();//�ⲿ�˿ڴ����ӳػ�ȡ����
private:
	ConnectionPool();

	bool loadConfigFile();//���������ļ�

	void produceConnectionTask();//�������߳����
	
	void scannerConnectionTask();//ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ����ӻ���

	string _ip;//mysql ip��ַ
	unsigned short _port;//mysql �˿ں� 3306
	string _username;//mysql ��¼�û���
	string _password;//MySQL ��¼����
	string _dbname; // �������ݿ�����
	int _initSize;//���ӳس�ʼ������
	int _maxSize;//���ӳ����������
	int _maxIdleTime;//���ӳ�������ʱ��
	int _connectionTimeout;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*> _connectionQue;//�洢MySQL���ӵĶ���
	mutex _queueMutex; //ά�����Ӷ��е��̰߳�ȫ������
	atomic<int> _connectionCnt;// ��¼������������connection���ӵ������� 
	condition_variable cv;//����ͨ��
};