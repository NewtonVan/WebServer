#include "dlist_timer.h"
#include "http_conn.h"

AscendTimerDoubleList::~AscendTimerDoubleList()
{
    UtilTimer *temp = head;
    while (temp)
    {
        /* code */
        head = temp->next;
        delete temp;
        temp = head;
    }
}

void AscendTimerDoubleList::AddTimer(UtilTimer *timer)
{
    if (timer == nullptr)
        return;
    if (head == nullptr)
    {
        head = tail = timer;
        return;
    }
    if (timer->expire < head->expire)
    {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return;
    }
    AddTimer(timer, head);
}
void AscendTimerDoubleList::AdjustTimer(UtilTimer *timer)
{
    if (!timer)
    {
        return;
    }
    if (!timer->next || (timer->expire < timer->next->expire))
    {
        return;
    }
    if (timer == head)
    {
        head = head->next;
        head->prev = NULL;
        timer->next = NULL;
        AddTimer(timer, head);
    }
    else
    {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        AddTimer(timer, timer->next);
    }
}
void AscendTimerDoubleList::DelTimer(UtilTimer *timer)
{
    if (!timer)
    {
        return;
    }
    if ((timer == head) && (timer == tail))
    {
        delete timer;
        head = NULL;
        tail = NULL;
        return;
    }
    if (timer == head)
    {
        head = head->next;
        head->prev = NULL;
        delete timer;
        return;
    }
    if (timer == tail)
    {
        tail = tail->prev;
        tail->next = NULL;
        delete timer;
        return;
    }
    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}
void AscendTimerDoubleList::Tick()
{
    if (!head)
    {
        return;
    }

    LOG_INFO("%s", "timer tick");
    Log::GetInstance()->Flush();
    time_t cur = time(NULL);
    UtilTimer *tmp = head;
    while (tmp)
    {
        if (cur < tmp->expire)
        {
            break;
        }
        tmp->callback_func(tmp->socket_timer_bind);
        head = tmp->next;
        if (head)
        {
            head->prev = NULL;
        }
        delete tmp;
        tmp = head;
    }
}

void AscendTimerDoubleList::AddTimer(UtilTimer *timer, UtilTimer *head)
{
    UtilTimer *prev = head;
    UtilTimer *tmp = prev->next;
    while (tmp)
    {
        if (timer->expire < tmp->expire)
        {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    if (!tmp)
    {
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
    }
}

void CallbackFunc(SocketTimerBind *stb)
{
    epoll_ctl(HttpConn::m_epollfd_, EPOLL_CTL_DEL, stb->sockfd, 0);
    assert(stb);
    close(stb->sockfd);
    HttpConn::m_user_count_--;
    // printf("close one client connection\n");
    // printf("m_user_count_: %d\n", HttpConn::m_user_count_);
    LOG_INFO("close fd %d", stb->sockfd);
    Log::GetInstance()->Flush();
}