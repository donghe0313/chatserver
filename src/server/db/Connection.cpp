#include <mymuduo/Logger.h>
#include "Connection.h"
#include <iostream>
using namespace std;

Connection::Connection()
{
	// ��ʼ�����ݿ�����
	_conn = mysql_init(nullptr);
}

Connection::~Connection()
{
	// �ͷ����ݿ�������Դ
	if (_conn != nullptr)
		mysql_close(_conn);
}

bool Connection::connect(string ip, unsigned short port, 
	string username, string password, string dbname)
{
	// �������ݿ�
	MYSQL *p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	if (p != nullptr)
    {
        // C��C++����Ĭ�ϵı����ַ���ASCII����������ã���MySQL����������������ʾ��
        mysql_query(_conn, "set names gbk");
        LOG_INFO ("connect mysql success!");
    }
    else
    {
        LOG_INFO ("connect mysql fail!");
    }

    return p;
}

bool Connection::update(string sql)
{
	// ���²��� insert��delete��update
	if (mysql_query(_conn, sql.c_str()))
	{
		string tmp = "����ʧ��:" + sql;
		LOG_INFO(tmp.c_str());
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(string sql)
{
	// ��ѯ���� select
	if (mysql_query(_conn, sql.c_str()))
	{
		string tmp = "��ѯʧ��:" + sql;
		LOG_INFO(tmp.c_str());
		return nullptr;
	}
	return mysql_use_result(_conn);
}

MYSQL* Connection::getConnection()
{
    return _conn;
}