#include "util.h"
#include "global.h"

// 计算文件数量
int countFilesInDirectory(const char *dirPath)
{
    DIR *dir = opendir(dirPath); // 打开指定目录
    if (dir == NULL)
    {
        perror("opendir"); // 打开目录失败，打印错误信息
        return -1;         // 返回错误码
    }

    struct dirent *entry;
    struct stat fileStat;
    int fileCount = 0;

    // 遍历目录中的每一项
    while ((entry = readdir(dir)) != NULL)
    {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // 构造完整的文件路径
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        // 获取文件状态信息
        if (stat(filePath, &fileStat) == 0)
        {
            // 如果是普通文件
            if (S_ISREG(fileStat.st_mode))
            {
                fileCount++;
            }
        }
        else
        {
            perror("stat"); // 获取文件状态失败，打印错误信息
        }
    }

    closedir(dir);    // 关闭目录
    return fileCount; // 返回文件数量
}

// 1:代表还需要继续读，0：代表：以读取至文件末尾|出错
int listFiles(DIR *dir, const char *dirPath, char *buffer)
{
    struct dirent *entry;
    struct stat fileStat;
    int fileCount = 0;
    // long totalSize = 0;
    char temp[1024];
    int ind = 0;

    // 遍历目录中的每一项
    while (ind <= maxSizeInBuff * fileInfo_size && (entry = readdir(dir)) != NULL)
    {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        // 构造完整的文件路径
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        // 获取文件的状态信息
        if (stat(filePath, &fileStat) == 0)
        {
            if (S_ISREG(fileStat.st_mode))
            { // 如果是普通文件

                fileCount++;
                // totalSize += fileStat.st_size; // 累加文件大小
                // 将文件创建时间（ctime）转换为可读格式
                char timeBuffer[26];
                struct tm *tm_info = localtime(&fileStat.st_ctime);
                strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tm_info);
                // 96 字节
                sprintf(temp, "文件名: %-20s | 大小: %-10ld 字节 | 创建时间: %s\n", entry->d_name, fileStat.st_size, timeBuffer);

                strncpy(buffer + ind, temp, strlen(temp) + 1);
                ind += strlen(temp);
            }
        }
        else
        {
            perror("stat"); // 获取文件状态失败，打印错误信息
        }
    }
    if (!fileCount)
        return 0;
    return 1;
}

// 构造用户信息
void Account_create()
{
    // accounts.push_back(Account("Anonymous", "123456"));
    // accounts.push_back(Account("user1", "123456"));
    // accounts.push_back(Account("user2", "123456"));
    strcpy(accounts[0].username, "Anonymous");
    strcpy(accounts[0].password, "123456");
    strcpy(accounts[1].username, "user1");
    strcpy(accounts[1].password, "123456");
    strcpy(accounts[2].username, "user2");
    strcpy(accounts[2].password, "123456");
}

// 处理 SIGINT 信号的回调函数
void handle_sigint(int sig)
{
    // 服务器
    if (server_sock != -1)
    {
        // 关闭套接字
        close(server_sock);
        printf("server Socket closed.\n");
    }
    for (int i = 0; i < client_count; i++)
    {
        if (client_sockets[i] != -1)
        {
            // 关闭套接字
            close(client_sockets[i]);
            printf("client Socket closed.\n");
        }
    }

    printf("Program terminated.\n");
    exit(0);
}