#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
// #include <signal.h>
// #include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
// #include <sys/socket.h>
#include <netinet/in.h>
// #include <arpa/inet.h>
#include <assert.h>
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
// #include <map>
// #include <string>
#include <signal.h>


extern int pipefd[2];

//对文件描述符设置非阻塞
int setnonblocking(int fd);

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void addfd(int epollfd, int fd, bool one_shot);

//从内核时间表删除描述符
void removefd(int epollfd, int fd);

//将事件重置为EPOLLONESHOT
void modfd(int epollfd, int fd, int ev);

void addsig(int sig, void(handler)(int), bool restart = true);

void sig_handler(int sig);

void show_error(int connfd, const char *info);

#endif