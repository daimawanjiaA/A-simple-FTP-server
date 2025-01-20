#include "global.h"
// #include "util.h"
#include "service.h"
#include <signal.h>

int main()
{

    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char command[3][SMALL_BUFFER_SIZE];

    // 注册 SIGINT 信号处理器
    signal(SIGINT, handle_sigint);

    // 创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }
    while (1)
    {
        printf("Please enter the listening port: ");
        scanf("%d", &SERVER_PORT);
        getchar();
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

        // 连接服务器
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("Connection failed");
            continue;
            // exit(1);
        }

        break;
    }

    // 进行登录
    if (!login(sock))
    {
        printf("Login failed. Exiting...\n");
        close(sock);
        return 0;
    }
    // printf("登录完毕~~~\n");
    system("clear"); // 清空终端消息

    recv(sock, buffer, BUFFER_SIZE, 0);
    // 接收欢迎信息
    printf("%s\n", buffer);
    help();

    // 用户输入命令并与服务器交互
    while (1)
    {
        printf("FTP> ");
        if (!get_command(command))
        {
            printf("Command input is incorrect.\n");
            continue;
        }

        if (strncmp(command[0], "GET", 3) == 0)
        {
            download_file(sock, command);
        }
        else if (strncmp(command[0], "PUT", 3) == 0)
        {
            upload_file(sock, command);
        }
        else if (strncmp(command[0], "LS", 4) == 0)
        {
            ls_request(sock, command);
        }
        else if (strncmp(command[0], "QUIT", 4) == 0)
        {
            send(sock, "QUIT", 4, 0);
            printf("Goodbye!\n");
            break;
        }
        else if (strncmp(command[0], "HELP", 4) == 0)
        {
            help();
        }
        else
        {
            printf("Unknown command\n");
            help();
        }
    }

    close(sock);
    return 0;
}
