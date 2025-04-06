#include "../include/ft_ping.h"

int init_socket(void) {
    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_fd == -1) {
        if (errno == EACCES) {
            printf("ft_ping: socket access error. Are you trying to ping broadcast ?\n");
        }
        fprintf(stderr, "socket creation failed\n");
        return -1;
    }

    struct timeval timeout = {1, 0};
    if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        fprintf(stderr, "Setsockopt timeout\n");
        close(sock_fd);
        return -1;
    }
    return sock_fd;
}

int resolve_target(const char *target) {
    struct addrinfo hints, *res = NULL; 
    memset(&hints, 0, sizeof(hints));  
    hints.ai_family = AF_INET;   
    hints.ai_socktype = SOCK_RAW;   
    hints.ai_protocol = IPPROTO_ICMP; 

    if (getaddrinfo(target, NULL, &hints, &res) != 0) {
        if (inet_aton(target, &g_ping_info.target_addr.sin_addr) == 0) {
            return -1; 
        }
    }

    if (res == NULL || res->ai_addr == NULL) {
        fprintf(stderr, "Error: No valid address found for target '%s'.\n", target);
        freeaddrinfo(res);
        return -1; 
    }

    if (sizeof(g_ping_info.target_addr) < sizeof(struct sockaddr_in)) {
        fprintf(stderr, "Error: Target address buffer is too small.\n");
        freeaddrinfo(res);
        return -1;  
    }

    memcpy(&g_ping_info.target_addr, res->ai_addr, sizeof(struct sockaddr_in));
    size_t target_len = strnlen(target, MAX_FQDN_LENGTH);
    if (target_len >= MAX_FQDN_LENGTH) {
        fprintf(stderr, "Error: Target hostname is too long (max %d characters).\n", MAX_FQDN_LENGTH - 1);
        freeaddrinfo(res);
        return -1;
    }

    strncpy(g_ping_info.original_fqdn, target, MAX_FQDN_LENGTH - 1);
    g_ping_info.original_fqdn[MAX_FQDN_LENGTH - 1] = '\0';
    freeaddrinfo(res);
    return 0;
}
