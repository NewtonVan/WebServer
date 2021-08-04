#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class Sem
{
private:
    sem_t m_sem_;
public:
    Sem()
    {
        if (0 != sem_init(&m_sem_, 0, 0)){
            throw std::exception();
        }
    }
    Sem(int num)
    {
        if (0 != sem_init(&m_sem_, 0, num)){
            throw std::exception();
        }
    }
    ~Sem()
    {
        sem_destroy(&m_sem_);
    }
    bool Wait()
    {
        return 0 == sem_wait(&m_sem_);
    }
    bool Post()
    {
        return 0 == sem_post(&m_sem_);
    }
};
class Locker
{
private:
    pthread_mutex_t m_mutex_;
public:
    Locker()
    {
        if (0 != pthread_mutex_init(&m_mutex_, NULL)){
            throw std::exception();
        }
    }
    ~Locker()
    {
        pthread_mutex_destroy(&m_mutex_);
    }
    bool Lock()
    {
        return 0 == pthread_mutex_lock(&m_mutex_);
    }
    bool Unlock()
    {
        return 0 == pthread_mutex_unlock(&m_mutex_);
    }
    pthread_mutex_t *Get()
    {
        return &m_mutex_;
    }
};
class Cond
{
private:
    pthread_cond_t m_cond_;
public:
    Cond()
    {
        if (0 != pthread_cond_init(&m_cond_, NULL)){
            throw std::exception();
        }
    }
    ~Cond()
    {
        pthread_cond_destroy(&m_cond_);
    }
    bool Wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        // TODO figure out the difference bewtween
        // You's version and Tiny web server
        ret = pthread_cond_wait(&m_cond_, m_mutex);

        return 0 == ret;
    }
    bool TimeWait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        // same as Cond.Wait
        ret = pthread_cond_timedwait(&m_cond_, m_mutex, &t);

        return 0 == ret;
    }
    bool Signal()
    {
        return 0 == pthread_cond_signal(&m_cond_);
    }
    bool BroadCast()
    {
        return 0 == pthread_cond_broadcast(&m_cond_);
    }
};
#endif