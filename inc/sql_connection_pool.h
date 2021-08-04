#ifndef SQL_CONNECTION_POOL_H
#define SQL_CONNECTION_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include <pthread.h>

#include "mysql.h"
#include "locker.h"
#include "log.h"

class ConnectionPool
{
public:
    MYSQL *GetConnection();
    bool ReleaseConnection(MYSQL *con);
    int GetFreeConn();
    void DestroyPool();

    static ConnectionPool *GetInstance();

    void Init(std::string url, std::string user, std::string passwd, std::string dbname, int port, int max_conn);

private:
    ConnectionPool();
    ~ConnectionPool();

    int max_conn_;
    int cur_conn_;
    int free_conn_;
    Locker lock_;
    std::list<MYSQL *> conn_list_;
    Sem reserve_;

public:
    std::string url_;
    std::string port_;
    std::string user_;
    std::string passwd_;
    std::string dbname_;
    int close_log_;
};

class ConnectionRAII
{
public:
    ConnectionRAII(MYSQL **con, ConnectionPool *conn_pool);
    ~ConnectionRAII();

private:
    MYSQL *conRAII_;
    ConnectionPool *poolRAII_;
};
#endif