#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <pthread.h>

#include "block_queue.h"

class Log
{
public:
    static Log *GetInstance()
    {
        static Log instance;
        return &instance;
    }

    static void *FlushLogThread(void *args)
    {
        Log::GetInstance()->AsyncWriteLog();
    }

    bool Init(const char *file_nm, int log_buf_sz = 8192, int split_lines = 5000000, int max_queue_size = 0);
    void WriteLog(int level, const char *format, ...);
    void Flush(void);

private:
    Log();
    virtual ~Log();
    void *AsyncWriteLog()
    {
        std::string single_log;
        while (log_queue_->Pop(single_log)){
            mutex_.Lock();
            fputs(single_log.c_str(), fp_);
            mutex_.Unlock();
        }
    }

private:
    char dir_name_[128];
    char log_name_[128];
    int split_lines_;
    int log_buf_size_;
    long long count_;
    int today_;
    FILE *fp_;
    char *buf_;
    BlockQueue<std::string> *log_queue_;
    bool is_async_;
    Locker mutex_;
};

#define LOG_DEBUG(format, ...) Log::GetInstance()->WriteLog(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::GetInstance()->WriteLog(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::GetInstance()->WriteLog(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::GetInstance()->WriteLog(3, format, ##__VA_ARGS__)

#endif