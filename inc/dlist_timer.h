#ifndef DLIST_TIMER_H
#define DLIST_TIMER_H

#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/epoll.h>
#include <assert.h>

#include "log.h"

class UtilTimer;

class SocketTimerBind
{
public:
    sockaddr_in address;
    int sockfd;
    UtilTimer *timer;
};

class UtilTimer
{
public:
    UtilTimer() : prev(nullptr), next(nullptr) {}

public:
    time_t expire;
    void (*callback_func)(SocketTimerBind *);
    SocketTimerBind *socket_timer_bind;
    UtilTimer *prev;
    UtilTimer *next;
};

// 超时时间升序的定时器双向链表
class AscendTimerDoubleList
{
public:
    AscendTimerDoubleList() : head(nullptr), tail(nullptr) {}
    ~AscendTimerDoubleList();

    void AddTimer(UtilTimer *timer);
    void AdjustTimer(UtilTimer *timer);
    void DelTimer(UtilTimer *timer);
    void Tick();

private:
    void AddTimer(UtilTimer *timer, UtilTimer *head);

private:
    UtilTimer *head;
    UtilTimer *tail;
};

void CallbackFunc(SocketTimerBind *stb);

#endif