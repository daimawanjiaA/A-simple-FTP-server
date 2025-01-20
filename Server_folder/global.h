#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>


#define MAX_CONNECTIONS 10    // 最大连接队列连接长度
#define BUFFER_SIZE 1024      // 大缓冲区容量（文件内容）
#define SMALL_BUFFER_SIZE 50  // 小缓冲区容量（文件名）

// 用户权限
#define ANONYMOUS_USER 0
#define USER1 1
#define USER2 0
// 信号量缓存
#define SIG_BUFFER_SIZE 10
// 缓冲区大小
#define BUFFER_SIZE 1024

// 端口号
extern int PORT;           
// 套接字
extern int server_sock, client_sock;
extern int client_sockets[100]; // 假设最多支持 100 个客户端连接
extern int client_count;

extern int fileInfo_size; // 文件描述信息大小
extern int maxSizeInBuff;  // 单缓冲区内文件描述最大容量
extern int user_counts; // 用户数量
// 响应类型
extern const int OK;
extern const int ERROR;
extern const char PERMIT_ERROR[];  // 权限问题
extern const char FILE_NO_EXIST[]; // 文件不存在
extern const char COMMAND_ERROR[]; // 命令有误
extern const char LOGIN_ERROR[];   // 登录出错

extern const char LOGIN_OK[]; // 登录成功
extern const char RE_OK[];    // 响应成功
extern const char RE_ERROR[]; // 响应失败
extern const char DEFAULT[];

extern pthread_rwlock_t rwlock;
// 认证用户结构体
struct Account
{
    char username[10];
    char password[10];
};
// 用户账户
extern struct Account accounts[];
