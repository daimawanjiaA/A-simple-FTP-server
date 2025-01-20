#!/bin/bash


# 检测系统中是否安装 GCC
if ! command -v gcc &> /dev/null; then
    echo "GCC 编译器未安装，正在安装..."
    # 对于 Debian/Ubuntu 系统
    sudo apt update && sudo apt install -y gcc make
    # echo "sudo apt update && sudo apt install -y gcc make"
else
    echo "GCC 编译器已安装"
fi

# 检查是否安装了 tmux
if ! command -v tmux &> /dev/null; then
    sudo apt install tmux
fi

# 定义 Client 和 Server 文件夹路径
Client_FOLDER="./Client_folder"
Server_FOLDER="./Server_folder"

# 编译项目
echo "开始编译项目..."

# 尝试在 Client 文件夹中执行 make
if [ -d "$Client_FOLDER" ]; then
    echo "正在编译 Client 文件夹中的项目..."
    cd "$Client_FOLDER"
    make clean
    make
    if [ $? -eq 0 ]; then
        echo "在 Client 文件夹中编译成功"
    else
        echo "Client 文件夹编译失败"
        exit 1
    fi
    cd ..
else
    echo "Client 文件夹不存在"
    exit 1
fi

# 在 Server 文件夹中执行 make
if [ -d "$Server_FOLDER" ]; then
    echo "正在编译 Server 文件夹中的项目..."
    cd "$Server_FOLDER"
    make clean
    make
    if [ $? -eq 0 ]; then
        echo "在 Server 文件夹中编译成功"
    else
        echo "Server 文件夹编译失败"
        exit 1
    fi
    cd ..
else
    echo "Server 文件夹不存在"
    exit 1
fi

# 创建 tmux 会话
SESSION_NAME="ftp_session"
tmux new-session -d -s $SESSION_NAME



# 在第一个窗口运行 client1
if [ -f "./client" ]; then
    echo "正在运行 FTP 客户端 1..."
    tmux new-window -t $SESSION_NAME -n 'client1'
    tmux send-keys -t $SESSION_NAME:0 './client' C-m
else
    echo "可执行文件 ./client 未生成，编译失败"
    exit 1
fi

# 在第二个窗口运行 client2
if [ -f "./client" ]; then
    echo "正在运行 FTP 客户端 2..."
    tmux new-window -t $SESSION_NAME -n 'client2'
    tmux send-keys -t $SESSION_NAME:1 './client' C-m
else
    echo "可执行文件 ./client 未生成，编译失败"
    exit 1
fi

# 在第三个窗口运行 client3
if [ -f "./client" ]; then
    echo "正在运行 FTP 客户端 3..."
    tmux new-window -t $SESSION_NAME -n 'client3'
    tmux send-keys -t $SESSION_NAME:2 './client' C-m
else
    echo "可执行文件 ./client 未生成，编译失败"
    exit 1
fi

# 在第四个窗口运行 server
if [ -f "./server" ]; then
    echo "正在运行 FTP 服务器..."
    tmux rename-window -t $SESSION_NAME:3 'server'
    tmux send-keys -t $SESSION_NAME:3 './server' C-m
else
    echo "可执行文件 ./server 未生成，编译失败"
    exit 1
fi

# 提示用户如何连接到 tmux 会话
echo "所有程序已启动。使用以下命令连接 tmux 会话以查看程序输出："
tmux attach-session -t $SESSION_NAME
