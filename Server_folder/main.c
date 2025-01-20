#include "service.h"
#include "util.h"
#include "global.h"
#include <pthread.h>
#include <signal.h>

int main()
{

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    Account_create();

    // 注册 SIGINT 信号处理器
    signal(SIGINT, handle_sigint);

    // 创建套接字
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Failed to create socket");
        exit(1);
    }
    while (1)
    {
        printf("Please enter the listening port: ");
        scanf("%d", &PORT);
        // 绑定端口
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("Bind failed");
            continue;
            // exit(1);
        }

        // 监听
        if (listen(server_sock, MAX_CONNECTIONS) < 0)
        {
            perror("Listen failed");
            continue;
            // exit(1);
        }
        break;
    }

    printf("FTP server is listening on port %d...\n", PORT);

    // 接受客户端连接并创建线程处理请求
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len)) > 0)
    {
        printf("Client connected.\n");
        if (pthread_create(&thread_id, NULL, client_handler, (void *)&client_sock) != 0)
        {
            perror("Failed to create thread");
            close(client_sock);
        }
        else
            client_sockets[client_count++] = client_sock;
    }

    close(server_sock);
    return 0;
}
