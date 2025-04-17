#include "../include/ft_ping.h"

void print_ping_header(int verbose)
{
    if (!verbose) {
        printf("PING %s (%s): %d data bytes\n",
        g_ping_info.original_fqdn,
        inet_ntoa(g_ping_info.target_addr.sin_addr),
        PACKET_SIZE);
    }
    else {
        pid_t pid = getpid();
        printf("PING %s (%s): %d data bytes, id 0x%04x = %d\n",
        g_ping_info.original_fqdn,
        inet_ntoa(g_ping_info.target_addr.sin_addr),
        PACKET_SIZE,
        pid,
        pid);
    }
}

void init_statistics(void) {
    g_ping_info.stats.transmitted = 0;
    g_ping_info.stats.received = 0;
    g_ping_info.stats.total_rtt = 0.0;
    g_ping_info.stats.sum_squares_rtt = 0.0;
    g_ping_info.stats.min_rtt = -1.0;
    g_ping_info.stats.max_rtt = 0.0;
}

void init_ping_session(void) {
    g_ping_info.session_id = getpid();
}

void display_usage() {
    printf("Try './ft_ping '-?'' for more information.\n");
}

void run_ping_loop(int verbose, int continuous) {
    int seq = 0;
    while (g_ping_info.running) {
        handle_single_ping(seq++, verbose, continuous);
        if (!continuous) {
            break;
        }
        sleep(1);
    }
}

int main(int argc, char **argv) 
{
    // parsing
    int verbose = 0;
    int help = 0;
    parse_arguments(argc, argv, g_ping_info.targets, &g_ping_info.target_count, &verbose, &help);
    if (help) 
    {
        display_usage();
        return EXIT_SUCCESS;
    }
    
    // initialisation
    signal(SIGINT, handle_signal);
    init_ping_session();
    init_statistics();

    // boucle de ping
    for (int i = 0; i < g_ping_info.target_count; i++) 
    {
        char *target = g_ping_info.targets[i];
        if (resolve_target(target) < 0) {
            fprintf(stderr, "ping: unknown host\n");
            continue;
        }
        g_ping_info.socket_fd = init_socket();
        if (g_ping_info.socket_fd < 0) {
            continue;
        }

        print_ping_header(verbose);
        gettimeofday(&g_ping_info.stats.start_time, NULL);
        
        if (i == 0)
            run_ping_loop(verbose, 1);  // Mode continu
        else
            {
                run_ping_loop(verbose, 0);  // Mode non continu
                printf("--- %s ping statistics ---\n", g_ping_info.original_fqdn);
                printf("1 packets transmitted, 0 packets received, 100%% packet loss\n");
            }
        close(g_ping_info.socket_fd);
        reset_stats();
    }
    return EXIT_SUCCESS;
}
