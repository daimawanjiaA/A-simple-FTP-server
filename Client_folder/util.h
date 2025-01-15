#pragma once
#include "global.h"

// 使用提示
void help();
// 命令分析
int get_command(char command[3][SMALL_BUFFER_SIZE]);

// 文件名提取
int get_filename(const char *input, char *filename);

// 处理 SIGINT 信号的回调函数
void handle_sigint(int sig);