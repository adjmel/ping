#include "../include/ft_ping.h"

struct ping_info g_ping_info = {
    .socket_fd = -1,
    .running = 1,
    .original_fqdn = {0},
    .target_count = 0,
    .current_target = 0,
    .session_id = 0
};

static void print_final_stats(void) {
    printf("\n--- %s ping statistics ---\n", g_ping_info.original_fqdn);
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n",
           g_ping_info.stats.transmitted,
           g_ping_info.stats.received,
           (g_ping_info.stats.transmitted == 0) ? 0 :
           (int)(((g_ping_info.stats.transmitted - g_ping_info.stats.received) * 100.0) /
                 g_ping_info.stats.transmitted));
    
    
     if (g_ping_info.stats.received > 0) {
            double avg = g_ping_info.stats.total_rtt / g_ping_info.stats.received;
            double stddev = sqrt(g_ping_info.stats.sum_squares_rtt / g_ping_info.stats.received - 
                               pow(avg, 2));
            printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
                   g_ping_info.stats.min_rtt, avg, g_ping_info.stats.max_rtt, stddev);
     }
}

void handle_signal(int sig) {
    if (sig == SIGINT) {
        g_ping_info.running = 0;
        gettimeofday(&g_ping_info.stats.end_time, NULL);
        print_final_stats();
    }
}
