#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <list>

#include "locker.h"
#include "sql_connection_pool.h"
#include "log.h"

template <typename T>
class ThreadPool
{
public:
    ThreadPool(ConnectionPool *connPool, int thread_number = 8, int max_request = 10000);
    ~ThreadPool();
    bool Append(T *request);

private:
    static void *Worker(void *arg);
    void Run();

private:
    int m_thread_number_;
    int m_max_requests_;
    pthread_t *m_threads_;
    std::list<T *> m_workqueue_;
    Sem m_queuestatus_;
    Locker m_queuelocker_;
    bool m_stop_;
    ConnectionPool *m_connPool;  //数据库
};

template <typename T>
ThreadPool<T>::ThreadPool(ConnectionPool *connPool, int thread_number, int max_request)
    : m_thread_number_(thread_number), m_max_requests_(max_request),
      m_threads_(NULL), m_stop_(false), m_connPool(connPool)
{
    if (thread_number <= 0 || max_request <= 0)
    {
        throw std::exception();
    }

    m_threads_ = new pthread_t[thread_number];
    if (!m_threads_)
        throw std::exception();
    int ret = 0;
    for (int i = 0; i < thread_number; i++)
    {
        ret = pthread_create(&m_threads_[i], NULL, Worker, this);
        if (ret != 0)
        {
            delete[] m_threads_;
            throw std::exception();
        }
        if (pthread_detach(m_threads_[i]) != 0)
        {
            delete[] m_threads_;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_threads_;
    m_stop_ = true;
}

template <typename T>
bool ThreadPool<T>::Append(T *request)
{
    m_queuelocker_.Lock();
    if (m_workqueue_.size() >= m_max_requests_)
    {
        m_queuelocker_.Unlock();
        return false;
    }
    m_workqueue_.push_back(request);
    m_queuelocker_.Unlock();
    m_queuestatus_.Post();
    return true;
}

template <typename T>
void *ThreadPool<T>::Worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    pool->Run();
    return pool;
}

template <typename T>
void ThreadPool<T>::Run()
{
    while (!m_stop_)
    {
        m_queuestatus_.Wait();
        m_queuelocker_.Lock();
        if (m_workqueue_.empty())
        {
            m_queuelocker_.Unlock();
            continue;
        }
        T *request = m_workqueue_.front();
        m_workqueue_.pop_front();
        m_queuelocker_.Unlock();
        if(request==NULL)
            continue;
        
        ConnectionRAII mysqlcon(&request->mysql_, m_connPool);
        request->Process();
    }
}

#endif