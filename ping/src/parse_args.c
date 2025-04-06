#include "../include/ft_ping.h"

bool validate_targetv4(const char *target) {
    struct in_addr addr;
    return inet_pton(AF_INET, target, &addr) == 1;
}

const char *skip_leading_zeros(const char *target) {
    while (*target == '0') {
        target++;
    }
    return target;
}

bool is_valid_numeric(const char *target) {
    target = skip_leading_zeros(target);

    size_t significant_digits = 0;
    int flag_letter = 0;
    for (; *target != '\0'; target++) {
        if (!isdigit(*target)) {
            flag_letter = 1;
        }
        significant_digits++;
        if (significant_digits > 10 && flag_letter != 1) {
            return false;
        }
    }
    return true;
}

bool validate_target(const char *target) {
    if (is_valid_numeric(target))
        return true;

    if (validate_targetv4(target))
        return true;

    return false;
}


void help_ping()
{
    printf("Usage: ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf("-v, verbose output\n");
    printf("-?, give this help list\n");
}

int parse_targets(int argc, char **argv, char **targets, int *target_count, int *help) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "-?") == 0) {
            continue;
        }

        if (validate_target(argv[i])) {
            if (*target_count < MAX_TARGETS) {
                targets[*target_count] = argv[i];
                (*target_count)++;
            } else {
                fprintf(stderr, "Error: Too many targets specified.\n");
                *help = 1;
                return -1;
            }
        } else {
            fprintf(stderr, "ping: unknown host\n");
            *help = 1;
            return -1;
        }
    }
    return 0;
}

void parse_arguments(int argc, char **argv, char **targets, int *target_count, int *verbose, int *help) {
    *verbose = 0;
    *help = 0;
    *target_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            *verbose = 1;
        } else if (strcmp(argv[i], "-?") == 0) {
            help_ping();
            exit(EXIT_SUCCESS);
        }
    }

    if (parse_targets(argc, argv, targets, target_count, help) == -1)
        exit(EXIT_SUCCESS);

    if (*target_count == 0) {
        fprintf(stderr, "ping: missing host operand\n");
        *help = 1;
    }

    if (*target_count > 0 && geteuid() != 0) {
        fprintf(stderr, "Error: This program must be run as root.\n");
        exit(EXIT_FAILURE);
    }
}