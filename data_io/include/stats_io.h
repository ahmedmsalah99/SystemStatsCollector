// data_reader.c — libproc2 version
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "globals.h"
#include <libproc2/misc.h>
#include <libproc2/pids.h>   // on some distros: #include <procps/pids.h>
#pragma once



 

typedef struct {
    pid_t pid;
    char cmd[32];
    uint32_t mem;  // Memory usage in MB
    float cpu;  // CPU usage in %
} process_stats;


extern process_stats top_processes[TOP_N];

static int file_exists(const char *filename);
static double timespec_diff_sec(struct timespec a, struct timespec b);

// Helper: insert into top_processes sorted by cpu descending
void insert_top_process(process_stats newp);

void sanitize_cmd(char *cmd);
int write_stats(process_stats stats);


process_stats read_stats();
    


// int main(int argc, char **argv) {
//     // if (argc != 2) {
//     //     fprintf(stderr, "usage: %s <pid>\n", argv[0]);
//     //     return 1;
//     // }
//     // pid_t pid = (pid_t)strtoul(argv[1], NULL, 10);

//     process_stats stats = read_stats();
//     for (int i = 0; i < top_count; i++) {
//         write_stats(top_processes[i]);
//     }

//     return 0;
// }
