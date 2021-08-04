#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <unistd.h>
// #include <signal.h>
// #include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
// #include <sys/socket.h>
#include <netinet/in.h>
// #include <arpa/inet.h>
// #include <assert.h>
#include <sys/stat.h>
#include <string.h>
// #include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
// #include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <string>

// #include "../lock/locker.h"
// #include "../CGImysql/sql_connection_pool.h"
// #include "../timer/lst_timer.h"
// #include "../log/log.h"
#include "locker.h"
#include "log.h"
#include "sql_connection_pool.h"

class HttpConn
{
public:
    static const int MaxFilenameLen = 200;
    static const int ReadBufferSize = 2048;
    static const int WriteBufferSize = 1024;
    enum HttpMethod
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    enum CheckState
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    enum HttpCode
    {
        // 希望更多数据
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    enum LineStatus
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    HttpConn() {}
    ~HttpConn() {}

public:
    // void Init(int sockfd, const sockaddr_in &addr, char *, int, int, std::string user, std::string passwd, std::string sqlname);
    void Init(int sockfd, const sockaddr_in &addr);
    void CloseConn(bool real_close = true);
    void Process();
    bool Read();
    bool Write();
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    void initmysql_result(ConnectionPool *connPool);
    // int timer_flag;
    // int improv;


private:
    void Init();
    HttpCode process_read();
    bool process_write(HttpCode ret);

    HttpCode parse_request_line(char *text);
    HttpCode parse_headers(char *text);
    HttpCode parse_content(char *text);
    HttpCode do_request();
    char *get_line() { return m_read_buf + m_start_line; };
    LineStatus parse_line();

    void unmap();
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:
    static int m_epollfd_;
    static int m_user_count_;
    MYSQL *mysql_;
    int m_state_;  //读为0, 写为1

private:
    int m_sockfd;
    sockaddr_in m_address;

    char m_read_buf[ReadBufferSize];
    int m_read_idx;
    int m_checked_idx;
    int m_start_line;
    char m_write_buf[WriteBufferSize];
    int m_write_idx;

    CheckState m_check_state;
    HttpMethod m_method;

    char m_real_file[MaxFilenameLen];
    char *m_url;
    char *m_version;
    char *m_host;
    int m_content_length;
    bool m_linger;

    char *m_file_address;
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;

    int cgi;        //是否启用的POST
    char *m_string; //存储请求头数?
    int bytes_to_send;
    int bytes_have_send;
    // char *doc_root;

    // map<string, string> m_users;
    // int m_TRIGMode;
    // int m_close_log;

    // char sql_user[100];
    // char sql_passwd[100];
    // char sql_name[100];
};

#endif