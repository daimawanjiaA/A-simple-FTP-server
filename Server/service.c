#include "service.h"

// 文件夹查看请求处理函数
int LS_handle(int client_sock, int permition)
{
    char buffer[BUFFER_SIZE];
    char dir_path[BUFFER_SIZE]; // 用于存储完整文件路径
    int file_count = 0;
    int now_count = 0;
    // 接收参数信息
    recv(client_sock, buffer, SMALL_BUFFER_SIZE, 0);
    /*
     *buffer,三种合法：DEFAULT，u1，u2
     *需要根据当前用户权限进行分配
     *匿名用户仅可查看u1的文件
     *user1仅可查看自己的文件和u2的文件
     *u2仅可查看自己的文件
     */
    // 文件路径处理
    char folder_name[50]; // 文件夹名称
    if (strcmp(buffer, "DEFAULT") == 0)
    {
        if (permition == 0)
            strcpy(folder_name, accounts[1].username);
        else
            strcpy(folder_name, accounts[permition].username);
    }
    else if (strcmp(buffer, "u1") == 0)
    {
        if (permition == 0 || permition == 1)
            strcpy(folder_name, accounts[1].username);
        else
        {
            send(client_sock, PERMIT_ERROR, SIG_BUFFER_SIZE, 0);
            return ERROR;
        }
    }
    else if (strcmp(buffer, "u2") == 0)
    {
        if (permition == 2)
            strcpy(folder_name, accounts[2].username);
        else
        {
            send(client_sock, PERMIT_ERROR, SIG_BUFFER_SIZE, 0);
            return ERROR;
        }
    }
    else
    {
        send(client_sock, COMMAND_ERROR, SIG_BUFFER_SIZE, 0);
        return ERROR;
    }
    // 继续判断权限
    // 均有查看权限

    // 得到文件目录路径
    snprintf(dir_path, sizeof(dir_path), "storage/%s", folder_name);
    printf("dir_path:%s\n", dir_path);
    send(client_sock, RE_OK, SIG_BUFFER_SIZE, 0);

    // 计算文件夹内文件数量
    file_count = countFilesInDirectory(dir_path);

    int converted_number = htonl(file_count * fileInfo_size); // 转换为网络字节序
    // 向客户机发送文件大小信息
    ssize_t sent_bytes = send(client_sock, &converted_number, sizeof(converted_number), 0);
    if (sent_bytes < 0)
    {
        perror("send failed");
        return -1;
    }

    DIR *dir = opendir(dir_path); // 打开指定的目录
    if (dir == NULL)
    {
        perror("opendir ERROR!!!\n"); // 打开目录失败，打印错误信息
        return 0;
    }
    while (listFiles(dir, dir_path, buffer))
    {
        send(client_sock, buffer, BUFFER_SIZE, 0);
    }
    closedir(dir); // 关闭目录
    printf("Directory retrieval successfully.\n");
    return OK;
}

// 文件上传请求处理函数
int upload_handle(int client_sock, int permition)
{
    char buffer[BUFFER_SIZE];
    char file_path[BUFFER_SIZE]; // 用于存储完整文件路径
    int file_size, bytes_received;
    FILE *fp;
    // char object[50];
    char folder_name[50];

    strcpy(folder_name, accounts[permition].username);

    // 读取文件名
    recv(client_sock, buffer, SMALL_BUFFER_SIZE, 0);

    char file_name[50]; // 文件名称
    strcpy(file_name, buffer);
    snprintf(folder_name, sizeof(folder_name), "storage/%s", accounts[permition].username);

    // 创建文件夹（如果不存在）
    struct stat st = {0};
    if (stat(folder_name, &st) == -1)
    {
        if (mkdir(folder_name, 0700) != 0)
        {
            perror("Failed to create directory");
        }
        else
            printf("Directory '%s' created successfully.\n", folder_name);
    }

    // 构造文件完整路径

    sprintf(file_path, "%s/%s", folder_name, file_name);
    printf("file_path: %s\n", file_path);

    // 打开文件
    fp = fopen(file_path, "wb");
    if (fp == NULL)
    {
        perror("Failed to open file");
        return -1;
    }
    // 接收文件大小
    ssize_t received_bytes = recv(client_sock, &file_size, sizeof(file_size), 0);
    if (received_bytes < 0)
    {
        perror("recv failed");
        return -1;
    }
    // 转换为主机字节序
    file_size = ntohl(file_size);
    printf("the size of file: %d\n", file_size);

    // 接收文件数据
    while (file_size > 0)
    {
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        // 在写操作之前先加锁，防止跟其他线程读写冲突
        pthread_rwlock_wrlock(&rwlock); // 获取写锁

        // 执行文件写操作
        fwrite(buffer, 1, bytes_received, fp);

        pthread_rwlock_unlock(&rwlock); // 释放写锁

        file_size -= bytes_received;
    }

    fclose(fp); // 关闭文件
    

    printf("File uploaded successfully.\n");
    return 1;
}

// 下载文件请求处理函数
int download_handle(int client_sock, int permition)
{
    char buffer[BUFFER_SIZE];
    char file_path[BUFFER_SIZE]; // 用于存储完整文件路径
    char object[SMALL_BUFFER_SIZE];
    char file_name[SMALL_BUFFER_SIZE]; // 文件名称
    int file_size;
    FILE *fp;

    // 接收参数信息
    recv(client_sock, buffer, BUFFER_SIZE, 0);
    // 提取命令对象和文件名称
    strcpy(object, buffer);
    strcpy(file_name, buffer + 1 + strlen(object));
    /*
     *buffer,三种合法：DEFAULT，u1，u2
     *需要根据当前用户权限进行分配
     *匿名用户仅可下载u1的文件
     *user1仅可下载自己的文件和u2的文件
     *u2仅可下载自己的文件
     */
    // 文件路径处理
    char folder_name[50]; // 文件夹名称

    // 未使用-u参数
    if (strcmp(object, "DEFAULT") == 0)
    {
        if (permition == 0)
            strcpy(folder_name, accounts[1].username);
        else
            strcpy(folder_name, accounts[permition].username);
    }
    else if (strcmp(object, "u1") == 0)
    {
        if (permition == 0 || permition == 1)
            strcpy(folder_name, accounts[1].username);
        else
        {
            send(client_sock, PERMIT_ERROR, SIG_BUFFER_SIZE, 0);
            return ERROR;
        }
    }
    else if (strcmp(object, "u2") == 0)
    {
        if (permition == 1 || permition == 2)
            strcpy(folder_name, accounts[2].username);
        else
        {
            send(client_sock, PERMIT_ERROR, SIG_BUFFER_SIZE, 0);
            return ERROR;
        }
    }
    else
    {
        send(client_sock, COMMAND_ERROR, SIG_BUFFER_SIZE, 0);
        return ERROR;
    }
    snprintf(file_path, sizeof(file_path), "storage/%s/%s", folder_name, file_name);
    // 打开文件
    fp = fopen(file_path, "rb");

    if (fp == NULL)
    {
        perror("File not found.");
        send(client_sock, FILE_NO_EXIST, SIG_BUFFER_SIZE, 0);
        return -1;
    }
    // 发送确认
    send(client_sock, RE_OK, SIG_BUFFER_SIZE, 0);

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    int converted_number = htonl(file_size); // 转换为网络字节序
    // 向客户机发送文件大小信息
    ssize_t sent_bytes = send(client_sock, &converted_number, sizeof(converted_number), 0);
    if (sent_bytes < 0)
    {
        perror("send failed");
        return -1;
    }

    // 发送文件内容
    while (file_size > 0)
    {
        // 读操作之前加读锁，可以让多个线程同时读取，但是跟写操作互斥
        pthread_rwlock_rdlock(&rwlock); // 获取读锁

        int bytes_read = fread(buffer, 1, sizeof(buffer), fp);

        pthread_rwlock_unlock(&rwlock); // 释放读锁
        if (file_size > BUFFER_SIZE)
            send(client_sock, buffer, BUFFER_SIZE, 0);
        else
            send(client_sock, buffer, file_size, 0);
        file_size -= bytes_read;
    }

    fclose(fp);
    printf("File downloaded successfully.\n");
    return OK;
}

// 处理客户端请求
void *client_handler(void *client_sock_ptr)
{
    int client_sock = *((int *)client_sock_ptr);
    char buffer[BUFFER_SIZE];
    int bytes_received;
    int permision = -1;

    // 发送欢迎信息
    // send(client_sock, "220 Welcome to Simple FTP Server\n", 32, 0);

    // 用户认证
    bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
    buffer[bytes_received] = '\0';
    printf("verify the client identity\n");
    for (int i = 0; i < user_counts; i++)
    {
        if (strcmp(accounts[i].username, buffer) == 0)
        {
            permision = i;
            break;
        }
    }
    // -----------------------------------------------------------------------------
    // permitsion：用户访问权限
    // 0：匿名用户，仅可查看并下载user1上传的文件，不可上传文件。
    // 1：user1，可查看并下载自己上传的文件，及下载user2上传的文件。
    // 2：user2，仅可查看并下载自己上传的文件。
    //
    // -----------------------------------------------------------------------------
    if (permision != -1)
    {
        send(client_sock, RE_OK, SIG_BUFFER_SIZE, 0);

        // send(client_sock, "Password: ", 10, 0);
        // todo 2 密码接收
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        buffer[bytes_received] = '\0';

        if (strcmp(accounts[permision].password, buffer) == 0)
        {
            printf("verify successlly\n");
            send(client_sock, LOGIN_OK, SIG_BUFFER_SIZE, 0);
            send(client_sock, "Welcome FTP SERVER!!!", BUFFER_SIZE, 0);

            // 接收FTP命令
            while (1)
            {
                // 接收客户端命令
                recv(client_sock, buffer, BUFFER_SIZE, 0);
                // buffer[sizeof(buffer)] = '\0';
                printf("client request: %s\n", buffer);

                // 下载
                if (strncmp(buffer, "GET", 3) == 0)
                {
                    // send(client_sock, &RE_OK, SIG_BUFFER_SIZE, 0);
                    download_handle(client_sock, permision);
                }
                // 上传
                else if (strncmp(buffer, "PUT", 3) == 0)
                {
                    if (permision == 0)
                    {
                        send(client_sock, &PERMIT_ERROR, SIG_BUFFER_SIZE, 0);
                        continue;
                    }
                    else
                        send(client_sock, &RE_OK, SIG_BUFFER_SIZE, 0);
                    upload_handle(client_sock, permision);
                }
                // 查看
                else if (strncmp(buffer, "LS", 3) == 0)
                {
                    LS_handle(client_sock, permision);
                }
                else if (strncmp(buffer, "QUIT", 4) == 0)
                {
                    // send(client_sock, "221 Goodbye.\n", 15, 0);
                    printf("client closed!!!\n");
                    break;
                }
                else
                {
                    send(client_sock, RE_ERROR, SIG_BUFFER_SIZE, 0);
                }
            }
        }
        else
        {
            send(client_sock, LOGIN_ERROR, SIG_BUFFER_SIZE, 0);
        }
    }
    else
    {
        send(client_sock, LOGIN_ERROR, SIG_BUFFER_SIZE, 0);
    }

    close(client_sock);
    return NULL;
}

