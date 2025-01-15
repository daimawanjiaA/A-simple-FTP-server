#pragma once
#include "global.h"
#include "util.h"
// 请求检索目录
int ls_request(int sock, char command[3][SMALL_BUFFER_SIZE]);

// 文件上传函数
int upload_file(int sock, char command[3][SMALL_BUFFER_SIZE]);

// 简单的文件下载函数
int download_file(int sock, char command[3][SMALL_BUFFER_SIZE]);

// 处理用户登录
int login(int sock);