#include "util.h"

// 使用提示
void help()
{
    printf("   GET [-user] (filename)    Download file. User 1: -u1, User 2: -u2, Default: no parameters.\n");
    printf("   PUT (filepath)            Upload file.\n");
    printf("   LS [-user]                View files. User 1: -u1, User 2: -u2, Default: no parameters.\n");
    printf("   HELP                      View command prompts.\n");
}

// 命令分析
int get_command(char command[3][SMALL_BUFFER_SIZE])
{
    char str1[1000];
    char *token;
    command[0][0] = '\0';
    command[1][0] = '\0';
    command[2][0] = '\0';
    if (fgets(str1, sizeof(str1), stdin) != NULL)
    {
        str1[strcspn(str1, "\n")] = '\0';

        token = strtok(str1, " ");
        int size = 0;
        while (token != NULL)
        {
            strcpy(command[size], token);
            token = strtok(NULL, " ");
            size++;
        }
        for (int i = 0; i < strlen(command[0]); i++)
        {
            if (command[0][i] - 'Z' > 0)
                command[0][i] -= 32;
        }

        // 消除-，GET 三个参数||LS 两个参数
        if (size == 3 || size == 2 && strcmp(command[0], "LS") == 0)
            strcpy(command[1], command[1] + 1);
        return OK;
    }
    else
    {
        return ERROR;
    }
}

// 文件名提取
int get_filename(const char *input, char *filename)
{
    int n = strlen(input);
    int i;
    for (i = n - 1; i >= 0; i--)
        if (input[i] == '/')
            break;
    i++;
    int len = n - i;
    strncpy(filename, input + i, len);
    filename[len] = '\0';
    return OK;
}

// 处理 SIGINT 信号的回调函数
void handle_sigint(int sig) {
    if (sock != -1) {
        // 关闭套接字
        close(sock);
        printf("Socket closed.\n");
    }
    printf("Program terminated.\n");
    exit(0);
}