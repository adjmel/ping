#include "../include/ft_ping.h"

void display_ping_results(char *buffer, int seq, double rtt, int verbose) {
    if (verbose) {
        print_verbose_info(buffer, seq, rtt);
    } else {
        print_info(buffer, seq, rtt);
    }
}

double calculate_rtt(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) * 1000.0 + (end->tv_usec - start->tv_usec) / 1000.0;
}

static ssize_t receive_packet(int socket_fd, char *buffer, struct sockaddr_in *recv_addr, socklen_t *addr_len) {
    ssize_t recv_len = recvfrom(socket_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *)recv_addr, addr_len);
    if (recv_len <= 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        } else {
            fprintf(stderr, "recvfrom failed\n");
            return -1;
        }
    }
    return recv_len;
}

static struct ip *parse_ip_header(char *buffer, int *ip_header_len) {
    struct ip *ip_header = (struct ip *)buffer;
    *ip_header_len = ip_header->ip_hl * 4;
    return ip_header;
}

static void handle_icmp_error(struct icmphdr *icmp_header, char *src_ip, int seq) {
    switch (icmp_header->type) {
        case ICMP_ECHOREPLY:
            printf("From %s icmp_seq=%d Echo Reply\n", src_ip, seq);
            break;
        case ICMP_DEST_UNREACH:
            switch (icmp_header->code) {
                case ICMP_NET_UNREACH:
                    printf("From %s icmp_seq=%d Destination Net Unreachable\n", src_ip, seq);
                    break;
                case ICMP_HOST_UNREACH:
                    printf("From %s icmp_seq=%d Destination Host Unreachable\n", src_ip, seq);
                    break;
                case ICMP_PROT_UNREACH:
                    printf("From %s icmp_seq=%d Destination Protocol Unreachable\n", src_ip, seq);
                    break;
                case ICMP_PORT_UNREACH:
                    printf("From %s icmp_seq=%d Destination Port Unreachable\n", src_ip, seq);
                    break;
                default:
                    printf("From %s icmp_seq=%d Destination Unreachable (Code %d)\n", src_ip, seq, icmp_header->code);
                    break;
            }
            break;
        case ICMP_SOURCE_QUENCH:
            printf("From %s icmp_seq=%d Source Quench\n", src_ip, seq);
            break;
        case ICMP_REDIRECT:
            printf("From %s icmp_seq=%d Redirect (change route)\n", src_ip, seq);
            break;
        case ICMP_ECHO:
            printf("From %s icmp_seq=%d Echo Request\n", src_ip, seq);
            break;
        case ICMP_TIME_EXCEEDED:
            printf("From %s icmp_seq=%d Time to live exceeded\n", src_ip, seq);
            break;
        case ICMP_PARAMETERPROB:
            printf("From %s icmp_seq=%d Parameter Problem\n", src_ip, seq);
            break;
        case ICMP_TIMESTAMP:
            printf("From %s icmp_seq=%d Timestamp Request\n", src_ip, seq);
            break;
        case ICMP_TIMESTAMPREPLY:
            printf("From %s icmp_seq=%d Timestamp Reply\n", src_ip, seq);
            break;
        case ICMP_INFO_REQUEST:
            printf("From %s icmp_seq=%d Information Request\n", src_ip, seq);
            break;
        case ICMP_INFO_REPLY:
            printf("From %s icmp_seq=%d Information Reply\n", src_ip, seq);
            break;
        case ICMP_ADDRESS:
            printf("From %s icmp_seq=%d Address Mask Request\n", src_ip, seq);
            break;
        case ICMP_ADDRESSREPLY:
            printf("From %s icmp_seq=%d Address Mask Reply\n", src_ip, seq);
            break;
        default:
            printf("From %s icmp_seq=%d Unknown ICMP type: %d\n", src_ip, seq, icmp_header->type);
            break;
    }
}

static int validate_parameters(char *buffer, struct sockaddr_in *recv_addr, socklen_t *addr_len) {
    if (!buffer || !recv_addr || !addr_len)
        return -1;
    return 0;
}

static int receive_ping_reply(char *buffer, struct sockaddr_in *recv_addr, socklen_t *addr_len, int seq) {
    if (validate_parameters(buffer, recv_addr, addr_len))
        return -1;

    ssize_t recv_len = receive_packet(g_ping_info.socket_fd, buffer, recv_addr, addr_len);
    if (recv_len <= 0)
        return -1;

    int ip_header_len;
    parse_ip_header(buffer, &ip_header_len);

    struct icmphdr *icmp_header = (struct icmphdr *)(buffer + ip_header_len);
    if (icmp_header->type == ICMP_DEST_UNREACH || icmp_header->type == ICMP_TIME_EXCEEDED) {
        //struct ip *original_ip_header = (struct ip *)(buffer + ip_header_len + sizeof(struct icmphdr));
        char src_ip[INET_ADDRSTRLEN];
        //inet_ntop(AF_INET, &(original_ip_header->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(recv_addr->sin_addr), src_ip, INET_ADDRSTRLEN);
        handle_icmp_error(icmp_header, src_ip, seq);
        return -1;
    }
    return 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void create_packet(struct icmphdr *icmp_hdr, int seq) {
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = g_ping_info.session_id;
    icmp_hdr->un.echo.sequence = seq;
    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = checksum(icmp_hdr, sizeof(struct icmphdr));
}

int send_and_receive_ping(int seq, struct timeval *start) {
    char packet[PACKET_SIZE] = {0};

    create_packet((struct icmphdr *)packet, seq);
    if (gettimeofday(start, NULL) == -1) {
        fprintf(stderr, "Error retrieving start time\n");
        return -1;
    }
    if (sendto(g_ping_info.socket_fd, packet, sizeof(struct icmphdr), 0,
               (struct sockaddr *)&g_ping_info.target_addr, sizeof(g_ping_info.target_addr)) <= 0) {
        printf("92 bytes from %s: Destination Net Unreachable\n", inet_ntoa(g_ping_info.target_addr.sin_addr));
        return -1;
    }
    return 0;
}


int validate_ip_address(const char *ip_str) {
    struct in_addr addr;
    return (inet_pton(AF_INET, ip_str, &addr) == 1) ? 0 : -1;
}

int handle_single_ping(int seq, int verbose, int is_first_target) {
    (void)is_first_target;
    char ip_str[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(g_ping_info.target_addr.sin_addr), ip_str, INET_ADDRSTRLEN);
    if (validate_ip_address(ip_str) != 0) {
        return -1;
    }

    struct timeval start, end;
    if (send_and_receive_ping(seq, &start) != 0) {
        g_ping_info.stats.transmitted++;
        return -1;
    }
    g_ping_info.stats.transmitted++;

    char buffer[PACKET_SIZE] = {0};
    struct sockaddr_in recv_addr;
    socklen_t addr_len = sizeof(recv_addr);
    int recv_result = receive_ping_reply(buffer, &recv_addr, &addr_len, seq);
    if (recv_result < 0) {
        return -1;
    }

    if (gettimeofday(&end, NULL) == -1) {
        fprintf(stderr, "Error retrieving end time\n");
        return -1;
    }

    double rtt = calculate_rtt(&start, &end);
    update_statistics(rtt);
    display_ping_results(buffer, seq, rtt, verbose);
    return 0;
}
