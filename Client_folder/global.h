#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SERVER_ADDR "127.0.0.1" // FTP服务器的IP地址

#define BUFFER_SIZE 1024     // 大缓冲区大小（文件内容）
#define SIG_BUFFER_SIZE 10   // 信号缓存大小
#define SMALL_BUFFER_SIZE 50 // 小缓冲区大小（文件名）


extern int SERVER_PORT; // 端口号
extern int sock; // 套接字描述符

// 响应类型
extern const int OK;
extern const int ERROR;
extern const char PERMIT_ERROR[];  // 220：权限问题
extern const char FILE_NO_EXIST[]; // 文件不存在
extern const char COMMAND_ERROR[]; // 命令有误
extern const char LOGIN_ERROR[];   // 登录出错
extern const char RE_OK[];         // 成功
extern const char RE_ERROR[];      // 失败
extern const char DEFAULT[];

extern const int fileInfo_size;
extern const int maxSizeInBuff;