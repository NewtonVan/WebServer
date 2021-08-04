#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "locker.h"

template <class T>
class BlockQueue
{
public:
    BlockQueue(int max_size = 1000)   
    {
        if (max_size <= 0){
            exit(-1);
        }

        max_size_ = max_size;
        array_ = new T[max_size];
        size_ = 0;
        front_ = -1;
        back_ = -1;
    }

    ~BlockQueue()
    {
        mutex_.Lock();
        if (NULL != array_){
            delete [] array_;
        }
        mutex_.Unlock();
    }
    
    void Clear()
    {
        mutex_.Lock();
        size_ = 0;
        front_ = -1;
        back_ = -1;
        mutex_.Unlock();
    }

    bool Full()
    {
        mutex_.Lock();
        if (size_ >= max_size_){
            mutex_.Unlock();
            return true;
        }
        mutex_.Unlock();
        return false;
    }

    bool Empty()
    {
        mutex_.Lock();
        if (0 == size_){
            mutex_.Unlock();
            return true;
        }
        mutex_.Unlock();
        return false;
    }

    bool Front(T &val)
    {
        mutex_.Lock();
        if (0 == size_){
            mutex_.Unlock();
            return false;
        }
        val = array_[front_];
        mutex_.Unlock();
        return true;
    }

    bool Back(T &val)
    {
        mutex_.Lock();
        if (0 == size_){
            mutex_.Unlock();
            return false;
        }
        val = array_[back_];
        mutex_.Unlock();
        return true;
    }

    int Size()
    {
        int tmp = 0;

        mutex_.Lock();
        tmp = size_;
        mutex_.Unlock();

        return tmp;
    }

    int MaxSize()
    {
        int tmp = 0;

        mutex_.Lock();
        tmp = max_size_;
        mutex_.Unlock();

        return tmp;
    }

    bool Push(const T &item)
    {
        mutex_.Lock();

        if (size_ >= max_size_){
            cond_.BroadCast();
            mutex_.Unlock();
            return false;
        }

        back_ = (back_+1) % max_size_;
        array_[back_] = item;

        ++size_;

        cond_.BroadCast();
        mutex_.Unlock();
    }

    bool Pop(T &item)
    {
        mutex_.Lock();

        while (size_ <= 0){
            if (!cond_.Wait(mutex_.Get())){
                mutex_.Unlock();
                return false;
            }
        }

        front_ = (front_+1) % max_size_;
        item = array_[front_];
        --size_;

        mutex_.Unlock();
        return true;
    }

    bool Pop(T &item, int ms_timeout)
    {
        struct timespec t = {0, 0};
        struct timeval now = {0, 0};
        gettimeofday(&now, NULL);

        mutex_.Lock();
        if (size_ <= 0){
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = (ms_timeout % 1000) * 1000;
            if (!cond_.TimeWait(mutex_.Get(), t)){
                mutex_.Unlock();
                return false;
            }
        }

        if (size_ <= 0){
            mutex_.Unlock();
            return false;
        }

        front_ = (front_+1)%max_size_;
        item = array_[front_];
        --size_;
        
        mutex_.Unlock();

        return true;
    }

private:
    Locker mutex_;
    Cond cond_;

    T *array_;
    int size_;
    int max_size_;
    int front_;
    int back_;
};

#endif