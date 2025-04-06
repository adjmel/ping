#include "../include/ft_ping.h"

void reset_stats(void) {
    memset(&g_ping_info.stats, 0, sizeof(struct ping_stats));
    g_ping_info.stats.min_rtt = -1.0;
    gettimeofday(&g_ping_info.stats.start_time, NULL);
}

void print_info(char *buffer, int seq, double rtt) {
    struct ip *ip_header = (struct ip *)buffer;
    int ttl = ip_header->ip_ttl;
    struct in_addr source_ip = ip_header->ip_src;

    printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
           inet_ntoa(source_ip),
           seq, ttl, rtt);
}

void print_verbose_info(char *buffer, int seq, double rtt) {
    struct ip *ip_header = (struct ip*)buffer;
    int ttl = ip_header->ip_ttl;
    struct in_addr source_ip = ip_header->ip_src;

    printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
           inet_ntoa(source_ip),
           seq, ttl, rtt);
}

void update_statistics(double rtt) {
    g_ping_info.stats.received++;
    g_ping_info.stats.total_rtt += rtt;
    g_ping_info.stats.sum_squares_rtt += (rtt * rtt);
    
    if (g_ping_info.stats.min_rtt == -1.0 || rtt < g_ping_info.stats.min_rtt) {
        g_ping_info.stats.min_rtt = rtt;
    }
    if (rtt > g_ping_info.stats.max_rtt) {
        g_ping_info.stats.max_rtt = rtt;
    }
}


    
