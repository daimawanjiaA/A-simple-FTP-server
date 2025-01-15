#include "service.h"
// 请求检索目录
int ls_request(int sock, char command[3][SMALL_BUFFER_SIZE])
{
    char dir[SMALL_BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int total_file_size = 0;
    char sign[10];
    int bytes_received;

    // todo1 发送请求类型
    send(sock, "LS", BUFFER_SIZE, 0);
    // todo1 发送请求对象
    if (strlen(command[1]) > 0)
        send(sock, command[1], SMALL_BUFFER_SIZE, 0);
    else
        send(sock, DEFAULT, SMALL_BUFFER_SIZE, 0);

    // todo2 接收响应
    recv(sock, &sign, SIG_BUFFER_SIZE, 0);
    if (strcmp(sign, RE_OK) != 0)
    {
        if (strcmp(sign, PERMIT_ERROR) == 0)
            printf("User request failed, please check user permissions.\n");
        else if (strcmp(sign, COMMAND_ERROR) == 0)
        {
            printf("Command error!!\n");
            help();
        }
        return ERROR;
    }
    // todo2 接收文件大小
    recv(sock, &total_file_size, sizeof(int), 0);

    // 网络序转主机序
    total_file_size = ntohl(total_file_size);
    if (total_file_size == -1)
    {
        printf("File not found on server.\n");
        return ERROR;
    }

    // 接收文件信息
    while (total_file_size > 0)
    {
        recv(sock, buffer, sizeof(buffer), 0);
        bytes_received = strlen(buffer);
        printf("%s", buffer);
        total_file_size -= bytes_received;
    }
    return OK;
}

// 文件上传函数
int upload_file(int sock, char command[3][SMALL_BUFFER_SIZE])
{
    char file_path[BUFFER_SIZE];
    char file_name[SMALL_BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int file_size;
    int bytes_read, bytes_sent;
    char sign[10]; // 接收返回信号
    FILE *fp;

    // 提取文件路径
    strcpy(file_path, command[1]);

    // 提取文件名称
    get_filename(file_path, file_name);

    // 打开文件
    fp = fopen(file_path, "rb");
    if (fp == NULL)
    {
        perror("File not found");
        return ERROR;
    }

    // 发送请求类型
    send(sock, "PUT", BUFFER_SIZE, 0);

    // 接收响应
    recv(sock, &sign, SIG_BUFFER_SIZE, 0);
    if (strcmp(sign, RE_OK) != 0)
    {
        if (strcmp(sign, PERMIT_ERROR) == 0)
            printf("User request failed, please check user permissions.\n");
        return ERROR;
    }

    // 发送文件名
    send(sock, file_name, SMALL_BUFFER_SIZE, 0);

    // printf("Enter the filename to upload: ");
    // scanf("%s", file_path);

    

    // 获取文件大小并发送
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // printf("filesize: %d\n", file_size);

    int converted_number = htonl(file_size); // 转换为网络字节序
    // 向服务器发送文件大小信息
    ssize_t sent_bytes = send(sock, &converted_number, sizeof(converted_number), 0);
    if (sent_bytes < 0)
    {
        printf("send failed!!!\n");
        return -1;
    }
    printf("filse size: %d Byte\n", file_size);
    // 发送文件内容
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        bytes_sent = send(sock, buffer, bytes_read, 0);
        // printf("sent file content: %s\n", buffer);
        if (bytes_sent < 0)
        {
            printf("Error sending file data!!\n");
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    printf("File uploaded successfully.\n");
    return OK;
}

// 简单的文件下载函数
int download_file(int sock, char command[3][SMALL_BUFFER_SIZE])
{
    char file_name[SMALL_BUFFER_SIZE];
    char file_path[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int file_size;
    int bytes_received;
    char sign[10];
    FILE *fp;

    // 发送请求类型
    send(sock, "GET", BUFFER_SIZE, 0);
    // 发送请求对象
    if (strlen(command[2]) == 0)
    {
        // 提取命令
        strncpy(buffer, DEFAULT, strlen(DEFAULT) + 1);
        // 提取文件名
        strcpy(file_name, command[1]);
        strncpy(buffer + strlen(DEFAULT) + 1, file_name, strlen(file_name) + 1);
        send(sock, buffer, BUFFER_SIZE, 0);
    }
    else
    {
        // 提取命令
        strncpy(buffer, command[1], strlen(command[1]) + 1);
        // 提取文件名
        strcpy(file_name, command[2]);
        strncpy(buffer + strlen(command[1]) + 1, file_name, strlen(file_name) + 1);
        send(sock, buffer, BUFFER_SIZE, 0);
    }
    // 构造下载的文件保存路径
    // sprintf(file_path,"../%s",file_name);
    strcpy(file_path,file_name);

    // 接收响应
    recv(sock, sign, SIG_BUFFER_SIZE, 0);
    if (strcmp(sign, RE_OK) != 0)
    {
        if (strcmp(sign, PERMIT_ERROR) == 0)
            printf("User request failed, please check user permissions.\n");
        else if (strcmp(sign, COMMAND_ERROR) == 0)
        {
            printf("Command error!!\n");
            help();
        }
        else if (strcmp(sign, FILE_NO_EXIST) == 0)
            printf("File not found!!\n");
        return ERROR;
    }

    // 接收文件大小
    recv(sock, &file_size, sizeof(int), 0);

    // 网络序转主机序
    file_size = ntohl(file_size);
    if (file_size == -1)
    {
        printf("File not found on server.\n");
        return ERROR;
    }
    // 创建待写入文件
    fp = fopen(file_path, "wb");
    if (fp == NULL)
    {
        perror("Failed to open file");
        return ERROR;
    }

    // 接收文件内容
    while (file_size > 0)
    {
        if (file_size > BUFFER_SIZE)
            bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        else
            bytes_received = recv(sock, buffer, file_size, 0);
        fwrite(buffer, 1, bytes_received, fp);
        file_size -= bytes_received;
    }

    fclose(fp);
    printf("File downloaded successfully.\n");
    return OK;
}

// 处理用户登录
int login(int sock)
{
    char username[BUFFER_SIZE + 1], password[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    username[0] = '\0';
    // 等待用户输入用户名
    printf("Username: ");
    scanf("%[^\n]", username);
    // 处理匿名登录的用户名
    if (strlen(username) == 0)
        strcpy(username, "Anonymous");

    // todo test
    // todo 1用户名发送
    send(sock, username, BUFFER_SIZE, 0);

    // 服务器返回用户名确认结果
    recv(sock, buffer, SIG_BUFFER_SIZE, 0);
    // printf("%s\n", buffer);
    if (strcmp(buffer, "530") == 0)
    {
        printf("user name error!\n");
        return ERROR;
    }

    // 等待用户输入密码
    printf("Password: ");
    scanf("%s", password);
    // todo 2 密码发送
    send(sock, password, BUFFER_SIZE, 0);

    // 服务器返回密码确认
    recv(sock, buffer, SIG_BUFFER_SIZE, 0);

    if (strcmp(buffer, "530") == 0)
    {
        printf("password error!\n");
        return 0; // 登录失败
    }
    printf("login successfully!\n");
    while (getchar() != '\n')
        ;     // 清除缓冲区
    return 1; // 登录成功
}