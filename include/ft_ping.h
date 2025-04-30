#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <math.h>
#include <netinet/ip.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef ICMP_NET_UNREACH
#define ICMP_NET_UNREACH       0 
#define ICMP_HOST_UNREACH      1 
#define ICMP_PORT_UNREACH      3 
#define ICMP_SOURCE_QUENCH     4  
#define ICMP_REDIRECT          5 
#define ICMP_ECHO              8    
#define ICMP_TIMESTAMPREPLY    14 
#define ICMP_INFO_REQUEST      15 
#define ICMP_INFO_REPLY        16  
#define ICMP_ADDRESS           17 
#define ICMP_ADDRESSREPLY      18 
#endif

#define MAX_FQDN_LENGTH        256
#define ICMP_PAYLOAD_SIZE      56 
#define PACKET_SIZE            ICMP_PAYLOAD_SIZE
#define MAX_TARGETS            10

#ifndef ICMP_TIME_EXCEEDED
#define ICMP_TIME_EXCEEDED     11
#endif

#ifndef ICMP_DEST_UNREACH
#define ICMP_DEST_UNREACH      3
#endif

struct ping_stats {
    int transmitted;
    int received;
    double total_rtt;
    double min_rtt;
    double max_rtt;
    double sum_squares_rtt;
    struct timeval start_time;
    struct timeval end_time;
};

struct ping_info {
    int socket_fd;
    volatile int running;
    char original_fqdn[MAX_FQDN_LENGTH];
    struct sockaddr_in target_addr;
    struct ping_stats stats;
    int target_count;
    char *targets[MAX_TARGETS];
    int current_target;
    uint16_t session_id;
};

extern struct ping_info g_ping_info;

void parse_arguments(int argc, char **argv, char **targets, int *target_count, int *verbose, int *help);
bool validate_targetv4(const char *target);
void handle_signal(int sig);
void init_ping_session(void);
int resolve_target(const char *target);
int init_socket(void);
int handle_single_ping(int seq, int is_first_target);
void print_statistics(void);
void help_ping(void);
void display_usage();
void update_statistics(double rtt);
void reset_stats(void);
unsigned short checksum(void *b, int len);
void create_packet(struct icmphdr *icmp_hdr, int seq);

#endif