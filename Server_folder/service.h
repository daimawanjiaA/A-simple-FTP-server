#pragma once
#include "global.h"
#include "util.h"

// 文件夹查看请求处理函数
int LS_handle(int client_sock, int permition);

// 文件上传请求处理函数
int upload_handle(int client_sock, int permition);

// 下载文件请求处理函数
int download_handle(int client_sock, int permition);

// 处理客户端请求
void *client_handler(void *client_sock_ptr);