#include "global.h"
// 响应类型
const int OK = 1;
const int ERROR = 0;
const char PERMIT_ERROR[] = "220";  // 220：权限问题
const char FILE_NO_EXIST[] = "221"; // 文件不存在
const char COMMAND_ERROR[] = "222"; // 命令有误
const char LOGIN_ERROR[] = "530";   // 登录出错
const char RE_OK[] = "502";         // 成功
const char RE_ERROR[] = "000";      // 失败
const char DEFAULT[] = "DEFAULT";

const int fileInfo_size = 96;
const int maxSizeInBuff = 9;
int SERVER_PORT; // 端口号
int sock;