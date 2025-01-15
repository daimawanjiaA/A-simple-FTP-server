#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

// 计算文件数量
int countFilesInDirectory(const char *dirPath);

// 1:代表还需要继续读，0：代表：以读取至文件末尾|出错
int listFiles(DIR *dir, const char *dirPath, char *buffer);

// 文件夹查看请求处理函数
int LS_handle(int client_sock, int permition);

// 构造用户信息
void Account_create();

// 处理 SIGINT 信号的回调函数
void handle_sigint(int sig);

// // 处理服务器 SIGINT 信号的回调函数
// void handle_server_sigint(int sig);

// // 处理客户 SIGINT 信号的回调函数
// void handle_client_sigint(int sig);