#include "global.h"

int fileInfo_size = 96; // 文件描述信息大小
int maxSizeInBuff = 9;  // 单缓冲区内文件描述最大容量
int user_counts = 3;    // 用户数量

// 套接字
int server_sock, client_sock;
int client_sockets[100]; // 假设最多支持 100 个客户端连接
int client_count = 0;
// 响应类型
const int OK = 1;
const int ERROR = 0;
const char PERMIT_ERROR[] = "220";  // 权限问题
const char FILE_NO_EXIST[] = "221"; // 文件不存在
const char COMMAND_ERROR[] = "222"; // 命令有误
const char LOGIN_ERROR[] = "530";   // 登录出错

const char LOGIN_OK[] = "520"; // 登录成功
const char RE_OK[] = "502";    // 响应成功
const char RE_ERROR[] = "000"; // 响应失败
const char DEFAULT[] = "DEFAULT";
struct Account accounts[3];

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER; // 初始化读写锁
