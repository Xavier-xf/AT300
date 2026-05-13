#ifndef _DB_SOCKET_H_
#define _DB_SOCKET_H_

// 统一错误码定义
#define DB_SOCKET_SUCCESS 0        /* 成功 */
#define DB_SOCKET_ERROR -1         /* 通用错误 */
#define DB_SOCKET_INVALID_PARAM -2 /* 无效参数 */
#define DB_SOCKET_TIMEOUT -3       /* 超时 */
#define DB_SOCKET_NETWORK_ERROR -4 /* 网络错误 */

// 统一类型定义
typedef int32_t db_socket_fd_t;       // 套接字文件描述符类型
typedef uint16_t db_socket_port_t;    // 端口号类型
typedef uint32_t db_socket_size_t;    // 数据大小类型
typedef int32_t db_socket_result_t;   // 函数返回结果类型
typedef uint32_t db_socket_timeout_t; // 超时时间类型（毫秒）

db_socket_result_t db_socket_close(db_socket_fd_t sock);

db_socket_result_t db_socket_multicast_join(db_socket_fd_t sock, const char *multicast_ip);

db_socket_result_t db_socket_multicast_join_if(db_socket_fd_t sock, const char *multicast_ip, const char *interface);

db_socket_fd_t db_socket_tcp_open(db_socket_port_t port, int client_max);

db_socket_fd_t db_socket_tcp_client_open(void);

db_socket_fd_t db_socket_tcp_accept(db_socket_fd_t sock, struct sockaddr_in *client_addr, db_socket_timeout_t timeout_ms);

db_socket_result_t db_socket_tcp_connect(db_socket_fd_t sock, const char *ip, db_socket_port_t port, db_socket_timeout_t timeout_ms);

db_socket_result_t db_socket_tcp_receive(db_socket_fd_t sock, unsigned char *data, db_socket_size_t data_len, db_socket_timeout_t timeout_ms);

db_socket_result_t db_socket_tcp_send(db_socket_fd_t sock, unsigned char *data, db_socket_size_t data_len, db_socket_timeout_t timeout_ms);

db_socket_fd_t db_socket_udp_open(db_socket_port_t port, int broadcast);

db_socket_result_t db_socket_udp_send(db_socket_fd_t sock, const char *data, db_socket_size_t data_len, const char *ip, db_socket_port_t port, db_socket_timeout_t timeout_ms);

db_socket_result_t db_socket_udp_receive(db_socket_fd_t sock, char *data, db_socket_size_t data_len, struct sockaddr_in *client_addr, db_socket_timeout_t timeout_ms);

#endif // _DB_SOCKET_H_