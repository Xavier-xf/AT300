/**
 * @file db_socket.c
 * @brief 套接字封装接口实现
 * @author DoorBell SDK Team
 * @version 1.0
 * @date 2025-04-03
 * 
 * 本文件实现了一系列套接字封装接口，包括TCP/UDP服务器和客户端的创建、连接、
 * 发送、接收等功能，以及组播相关操作。所有接口都提供了统一的错误码和类型定义，
 * 提高了代码的可维护性和可移植性。
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <stdbool.h>
#include "db_common.h"
#include "db_socket.h"

/**
 * @brief 设置套接字的阻塞/非阻塞模式
 * @param sock 套接字文件描述符
 * @param blocking 是否启用阻塞模式：true表示阻塞模式，false表示非阻塞模式
 * @return 成功返回DB_SOCKET_SUCCESS，失败返回对应的错误码
 */
static db_socket_result_t db_socket_set_blocking(db_socket_fd_t sock, bool blocking)
{
    if (sock < 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    int ret;
    int flags = fcntl(sock, F_GETFL);
    if (flags == -1)
    {
        return DB_SOCKET_ERROR;
    }
    
    if (blocking)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }
    
    ret = fcntl(sock, F_SETFL, flags);
    if (ret == -1)
    {
        return DB_SOCKET_ERROR;
    }

    return DB_SOCKET_SUCCESS;
}

/**
 * @brief 等待套接字可读
 * @param sock 套接字文件描述符
 * @param wait_ms 等待超时时间（毫秒）
 * @return 成功返回DB_SOCKET_SUCCESS，超时返回DB_SOCKET_TIMEOUT，失败返回DB_SOCKET_ERROR
 */
static db_socket_result_t db_socket_select_read(db_socket_fd_t sock, db_socket_timeout_t timeout_ms)
{
    int ret = -1;
    fd_set fdset;
    struct timeval timeout;

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = timeout_ms % 1000 * 1000;

    do
    {
        ret = select(sock + 1, &fdset, NULL, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);

    if (ret == 0)
    {
        return DB_SOCKET_TIMEOUT;
    }
    else if (ret == 1 && FD_ISSET(sock, &fdset))
    {
        return DB_SOCKET_SUCCESS;
    }

    return DB_SOCKET_ERROR;
}

/**
 * @brief 等待套接字可写
 * @param sock 套接字文件描述符
 * @param wait_ms 等待超时时间（毫秒）
 * @return 成功返回DB_SOCKET_SUCCESS，超时返回DB_SOCKET_TIMEOUT，失败返回DB_SOCKET_ERROR
 */
static db_socket_result_t db_socket_select_write(db_socket_fd_t sock, db_socket_timeout_t timeout_ms)
{
    int ret = -1;
    fd_set fdset;
    struct timeval timeout;

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = timeout_ms % 1000 * 1000;

    do
    {
        ret = select(sock + 1, NULL, &fdset, NULL, &timeout);
    } while (ret < 0 && errno == EINTR);

    if (ret == 0)
    {
        return DB_SOCKET_TIMEOUT;
    }
    else if (ret == 1 && FD_ISSET(sock, &fdset))
    {
        return DB_SOCKET_SUCCESS;
    }

    return DB_SOCKET_ERROR;
}

/**
 * @brief 创建并配置套接字
 * @param type 套接字类型（SOCK_STREAM或SOCK_DGRAM）
 * @param port 端口号
 * @param dev 网络接口名称（可选，NULL表示不指定）
 * @return 成功返回套接字文件描述符，失败返回对应的错误码
 */
static db_socket_fd_t db_socket_open(int type, int port, const char *dev)
{
    int opt = 1;
    int loop = 0;
    int sock = -1;
    struct sockaddr_in addr;

    // 参数验证
    if (port <= 0 || port > 65535)
    {
        db_log_error("Invalid port: %d", port);
        return DB_SOCKET_INVALID_PARAM;
    }

    // 创建套接字
    if ((sock = socket(AF_INET, type, 0)) < 0)
    {
        db_log_error("Failed to create socket: %s", strerror(errno));
        goto finish;
    }
    
    // 指定网卡
    if (dev && setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, dev, strlen(dev) + 1) < 0)
    {
        db_log_error("Failed to bind to device %s: %s", dev, strerror(errno));
        goto finish;
    }
    
    // 地址复用，确保服务器重启时不受TIME_WAIT影响
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) < 0)
    {
        db_log_error("Failed to set SO_REUSEADDR: %s", strerror(errno));
        goto finish;
    }
    
    // 端口复用
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(opt)) < 0)
    {
        db_log_error("Failed to set SO_REUSEPORT: %s", strerror(errno));
        goto finish;
    }
    
    // 绑定端口
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        db_log_error("Failed to bind to port %d: %s", port, strerror(errno));
        goto finish;
    }
    
    // 关闭组播回环
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
    {
        db_log_error("Failed to set IP_MULTICAST_LOOP: %s", strerror(errno));
        goto finish;
    }
    
    return sock;
finish:
    if (sock >= 0)
    {
        close(sock);
    }
    return DB_SOCKET_ERROR;
}

/**
 * @brief 关闭套接字
 * @param sock 套接字文件描述符
 * @return 成功返回DB_SOCKET_SUCCESS，失败返回DB_SOCKET_ERROR
 */
db_socket_result_t db_socket_close(db_socket_fd_t sock)
{
    if (sock < 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    if (close(sock) < 0)
    {
        db_log_error("Failed to close socket: %s", strerror(errno));
        return DB_SOCKET_ERROR;
    }
    
    return DB_SOCKET_SUCCESS;
}

/**
 * @brief 加入组播组
 * @param sock UDP套接字文件描述符
 * @param multicast_ip 组播IP地址
 * @return 成功返回DB_SOCKET_SUCCESS，失败返回对应的错误码
 */
db_socket_result_t db_socket_multicast_join(db_socket_fd_t sock, const char *multicast_ip)
{
    // 调用带接口参数的版本，使用默认接口
    return db_socket_multicast_join_if(sock, multicast_ip, NULL);
}

/**
 * @brief 指定网络接口加入组播组
 * @param sock UDP套接字文件描述符
 * @param multicast_ip 组播IP地址
 * @param interface 网络接口名称
 * @return 成功返回DB_SOCKET_SUCCESS，失败返回对应的错误码
 */
db_socket_result_t db_socket_multicast_join_if(db_socket_fd_t sock, const char *multicast_ip, const char *interface)
{
    if (sock < 0 || multicast_ip == NULL)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    // 加入组播
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    if (mreq.imr_multiaddr.s_addr == INADDR_NONE)
    {
        db_log_error("Invalid multicast IP: %s", multicast_ip);
        return DB_SOCKET_INVALID_PARAM;
    }
    
    if (interface != NULL)
    {
        // 通过网络接口名称获取IP地址
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd < 0)
        {
            db_log_error("Failed to create socket for interface lookup: %s", strerror(errno));
            return DB_SOCKET_ERROR;
        }
        
        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        
        if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
        {
            db_log_error("Failed to get IP address for interface %s: %s", interface, strerror(errno));
            close(fd);
            return DB_SOCKET_ERROR;
        }
        
        close(fd);
        
        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        mreq.imr_interface.s_addr = addr->sin_addr.s_addr;
        db_log_info("Joining multicast group %s on interface %s (%s)", 
               multicast_ip, interface, inet_ntoa(addr->sin_addr));
    }
    else
    {
        // 使用默认接口
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        db_log_info("Joining multicast group %s on all interfaces", multicast_ip);
    }
    
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq, sizeof(mreq)) < 0)
    {
        db_log_error("Failed to join multicast group %s: %s", multicast_ip, strerror(errno));
        return DB_SOCKET_ERROR;
    }
    
    return DB_SOCKET_SUCCESS;
}

/**
 * @brief 创建TCP服务器套接字
 * @param port 监听端口号
 * @param client_max 最大连接数
 * @return 成功返回套接字文件描述符，失败返回对应的错误码
 */
db_socket_fd_t db_socket_tcp_open(db_socket_port_t port, int client_max)
{
    if (port <= 0 || port > 65535)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    int sock = -1;
    // 创建tcp套接字
    if ((sock = db_socket_open(SOCK_STREAM, port, NULL)) < 0)
    {
        goto finish;
    }
    // 服务器监听客户端连接请求
    if (client_max > 0 && listen(sock, client_max) < 0)
    {
        goto finish;
    }

    return sock;
finish:
    if (sock >= 0)
    {
        close(sock);
    }
    return DB_SOCKET_ERROR;
}

/**
 * @brief 创建TCP客户端套接字
 * @return 成功返回套接字文件描述符，失败返回对应的错误码
 */
db_socket_fd_t db_socket_tcp_client_open(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return DB_SOCKET_ERROR;
    }
    
    // 设置TCP_NODELAY选项，禁用Nagle算法
    int nodelay = 1;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0)
    {
        close(sock);
        return DB_SOCKET_ERROR;
    }
    
    return sock;
}

/**
 * @brief 接受TCP连接
 * @param sock 监听套接字文件描述符
 * @param client_addr 客户端地址信息（可选，NULL表示不获取）
 * @param timeout_ms 接收超时时间（毫秒）
 * @return 成功返回新的连接套接字文件描述符，失败返回对应的错误码
 */
db_socket_fd_t db_socket_tcp_accept(db_socket_fd_t sock, struct sockaddr_in *client_addr, db_socket_timeout_t timeout_ms)
{
    if (sock < 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    db_socket_fd_t client_fd = -1;
    socklen_t len = sizeof(struct sockaddr_in);

    if (timeout_ms > 0)
    {
        db_socket_result_t select_ret = db_socket_select_read(sock, timeout_ms);
        if (select_ret != DB_SOCKET_SUCCESS)
        {
            return select_ret; // 返回具体的错误码
        }
    }
    
    client_fd = accept(sock, (struct sockaddr *)client_addr, &len);
    if (client_fd < 0)
    {
        db_log_error("Failed to accept client connection: %s", strerror(errno));
        return DB_SOCKET_ERROR;
    }
    
    if (client_addr != NULL)
    {
        db_log_info("New client connected: %s:%d", 
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
    }
    
    return client_fd;
}

/**
 * @brief 建立TCP连接
 * @param sock TCP客户端套接字文件描述符
 * @param ip 服务器IP地址
 * @param port 服务器端口号
 * @param wait_ms 连接超时时间（毫秒）
 * @return 成功返回DB_SOCKET_SUCCESS，超时返回DB_SOCKET_TIMEOUT，失败返回对应的错误码
 */
db_socket_result_t db_socket_tcp_connect(db_socket_fd_t sock, const char *ip, db_socket_port_t port, db_socket_timeout_t timeout_ms)
{
    if (sock < 0 || ip == NULL || port <= 0 || port > 65535)
    {
        return DB_SOCKET_INVALID_PARAM;
    }

    int ret = 0;
    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(port);
    ser_addr.sin_addr.s_addr = inet_addr(ip);

    if (timeout_ms > 0) {
        // 设置为非阻塞模式
        if (db_socket_set_blocking(sock, false) != DB_SOCKET_SUCCESS) {
            return DB_SOCKET_ERROR;
        }
    }

    ret = connect(sock, (struct sockaddr *)&ser_addr, sizeof(ser_addr));

    if (ret < 0)
    {
        if (errno == EINPROGRESS && timeout_ms > 0) {
        // 非阻塞模式下连接正在进行，等待完成
        if (db_socket_select_write(sock, timeout_ms) == DB_SOCKET_SUCCESS) {
            int error = 0;
            socklen_t len = sizeof(error);
            if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
                return DB_SOCKET_ERROR;
            } else if (error == 0) {
                return DB_SOCKET_SUCCESS;
            } else {
                // 连接失败，设置errno为实际错误码
                errno = error;
                return DB_SOCKET_ERROR;
            }
        } else {
            // 超时
            return DB_SOCKET_TIMEOUT;
        }
    } else {
        // 其他错误
        return DB_SOCKET_ERROR;
    }
    }
    // 如果ret == 0，连接成功，无需处理

    if (timeout_ms > 0) {
        // 恢复为阻塞模式
        db_socket_set_blocking(sock, true);
    }

    return DB_SOCKET_SUCCESS;
}

/**
 * @brief 接收TCP数据
 * @param sock TCP连接套接字文件描述符
 * @param data 接收缓冲区
 * @param data_len 接收缓冲区长度
 * @param timeout_ms 接收超时时间（毫秒）
 * @return 成功返回实际接收的数据长度，超时返回DB_SOCKET_TIMEOUT，失败返回对应的错误码
 */
db_socket_result_t db_socket_tcp_receive(db_socket_fd_t sock, unsigned char *data, db_socket_size_t data_len, db_socket_timeout_t timeout_ms)
{
    if (sock < 0 || data == NULL || data_len <= 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    db_socket_result_t select_ret = db_socket_select_read(sock, timeout_ms);
    if (select_ret != DB_SOCKET_SUCCESS)
    {
        return select_ret; // 返回具体的错误码
    }
    
    int ret = recv(sock, data, data_len, 0);
    if (ret < 0)
    {
        return DB_SOCKET_NETWORK_ERROR;
    }
    
    return ret; // 返回实际接收到的数据大小
}

/**
 * @brief 发送TCP数据
 * @param sock TCP连接套接字文件描述符
 * @param data 发送缓冲区
 * @param data_len 发送数据长度
 * @param timeout_ms 发送超时时间（毫秒）
 * @return 成功返回DB_SOCKET_SUCCESS，超时返回DB_SOCKET_TIMEOUT，失败返回对应的错误码
 */
db_socket_result_t db_socket_tcp_send(db_socket_fd_t sock, unsigned char *data, db_socket_size_t data_len, db_socket_timeout_t timeout_ms)
{
    if (sock < 0 || data == NULL || data_len <= 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    int ret = 0;
    int send_len = 0;

    while (send_len < data_len)
    {
        db_socket_result_t select_ret = db_socket_select_write(sock, timeout_ms);
        if (select_ret != DB_SOCKET_SUCCESS)
        {
            return select_ret; // 返回具体的错误码
        }

        ret = send(sock, &data[send_len], data_len - send_len, 0);
        if (ret <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }
            return DB_SOCKET_NETWORK_ERROR;
        }
        send_len += ret;
    }
    return DB_SOCKET_SUCCESS;
}

/**
 * @brief 创建UDP套接字
 * @param port 端口号
 * @param broadcast 是否允许广播（1允许，0不允许）
 * @return 成功返回套接字文件描述符，失败返回对应的错误码
 */
db_socket_fd_t db_socket_udp_open(db_socket_port_t port, int broadcast)
{
    int sock = db_socket_open(SOCK_DGRAM, port, NULL);
    if (sock < 0)
    {
        return DB_SOCKET_ERROR;
    }

    if (broadcast)
    {
        int optval = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&optval, sizeof(int)) < 0)
        {
            db_log_error("Failed to set SO_BROADCAST: %s", strerror(errno));
            db_socket_close(sock);
            return DB_SOCKET_ERROR;
        }
    }

    return sock;
}

/**
 * @brief 发送UDP数据
 * @param sock UDP套接字文件描述符
 * @param data 发送缓冲区
 * @param data_len 发送数据长度
 * @param ip 目标IP地址
 * @param port 目标端口号
 * @param timeout_ms 发送超时时间（毫秒）
 * @return 成功返回DB_SOCKET_SUCCESS，超时返回DB_SOCKET_TIMEOUT，失败返回对应的错误码
 */
db_socket_result_t db_socket_udp_send(db_socket_fd_t sock, const char *data, db_socket_size_t data_len, const char *ip, db_socket_port_t port, db_socket_timeout_t timeout_ms)
{
    if (sock < 0 || data == NULL || data_len <= 0 || ip == NULL || port <= 0 || port > 65535)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    db_socket_result_t select_ret = db_socket_select_write(sock, timeout_ms);
    if (select_ret != DB_SOCKET_SUCCESS)
    {
        return select_ret; // 返回具体的错误码
    }

    int ret = sendto(sock, data, data_len, 0, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        return DB_SOCKET_NETWORK_ERROR;
    }
    
    return (ret == data_len) ? DB_SOCKET_SUCCESS : DB_SOCKET_NETWORK_ERROR;
}

/**
 * @brief 接收UDP数据
 * @param sock UDP套接字文件描述符
 * @param data 接收缓冲区
 * @param data_len 接收缓冲区长度
 * @param client_addr 客户端地址信息（可选，NULL表示不获取）
 * @param timeout_ms 接收超时时间（毫秒）
 * @return 成功返回实际接收的数据长度，超时返回DB_SOCKET_TIMEOUT，失败返回对应的错误码
 */
db_socket_result_t db_socket_udp_receive(db_socket_fd_t sock, char *data, db_socket_size_t data_len, struct sockaddr_in *client_addr, db_socket_timeout_t timeout_ms)
{
    if (sock < 0 || data == NULL || data_len <= 0)
    {
        return DB_SOCKET_INVALID_PARAM;
    }
    
    socklen_t addr_len = sizeof(struct sockaddr_in);
    db_socket_result_t select_ret = db_socket_select_read(sock, timeout_ms);
    if (select_ret != DB_SOCKET_SUCCESS)
    {
        return select_ret; // 返回具体的错误码
    }
    
    int ret = recvfrom(sock, data, data_len, 0, (struct sockaddr *)client_addr, &addr_len);
    if (ret < 0)
    {
        return DB_SOCKET_NETWORK_ERROR;
    }
    
    return ret; // 返回实际接收到的数据大小
}