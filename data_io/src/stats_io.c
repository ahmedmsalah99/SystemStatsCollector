// data_reader.c — libproc2 version
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libproc2/misc.h>
#include <libproc2/pids.h>   // on some distros: #include <procps/pids.h>
#include "globals.h"
#include "stats_io.h"

int top_count = 0;
process_stats top_processes[TOP_N];
char LOGGING_DIR[32] = "./";
double DELAY_BETWEEN_CALLS = 10;

static int file_exists(const char *filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}
static double timespec_diff_sec(struct timespec a, struct timespec b) {
    return (a.tv_sec - b.tv_sec) + (a.tv_nsec - b.tv_nsec) / 1e9;
}

// Helper: insert into top_processes sorted by cpu descending
void insert_top_process(process_stats newp) {
    if (top_count < TOP_N) {
        // Add and increment
        top_processes[top_count++] = newp;
    } else {
        // Only insert if better than the smallest
        if (newp.cpu < top_processes[top_count - 1].cpu) {
            return; // skip, not better
        }
        top_processes[top_count - 1] = newp;
    }

    // Sort descending by CPU
    for (int i = top_count - 1; i > 0; i--) {
        if (top_processes[i].cpu > top_processes[i - 1].cpu) {
            process_stats tmp = top_processes[i];
            top_processes[i] = top_processes[i - 1];
            top_processes[i - 1] = tmp;
        } else {
            break;
        }
    }
}




void sanitize_cmd(char *cmd) {
    for (char *p = cmd; *p; p++) {
        if (*p == '/' || *p == ' ') {
            *p = '_';   // replace with underscore
        }
    }
}
int write_stats(process_stats stats) {
    if(stats.cpu==0){
        return 0;
    }

    char filename[256];
    FILE *file;
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timestamp[20];
    sanitize_cmd(stats.cmd);
    snprintf(filename, sizeof(filename), "%s/%s_%d.log", LOGGING_DIR, stats.cmd,(int)stats.pid);
    printf("%s %s\n",stats.cmd,filename);
    if (!file_exists(filename)) {
        file = fopen(filename, "w");
        printf("%s %s\n",stats.cmd,filename);
        if (!file) { perror("creating log file"); return -1; }
        fprintf(file, "timestamp\tmem(MB)\tcpu(%%)\n");
        fclose(file);
    }

    file = fopen(filename, "a");
    if (!file) { perror("opening log file"); return -1; }

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d-%H:%M:%S", tm);
    fprintf(file, "%s\t%u\t%.2f\n", timestamp, stats.mem, stats.cpu);
    fclose(file);
    return 0;
}


process_stats read_stats() {
    process_stats out = {0, 0};

    // Choose which fields we want, and record their indexes.
    // enum { I_PID, I_MEM_RES, I_UTIL, I__N };
    enum rel_items { rel_pid, rel_mem, rel_ticks_all, rel_cmd };
    enum pids_item items[4] = {
        PIDS_ID_PID,      // s_int
        PIDS_MEM_RES,     // ul_int (KiB)
        PIDS_TICS_ALL,  
        PIDS_CMD
    };

    struct pids_info *info = NULL;
    struct pids_info *info2 = NULL;
    if (procps_pids_new(&info, items, 4) < 0) return out;
    if (procps_pids_new(&info2, items, 4) < 0) return out;

    // Fetch one stack (there should be at most one due to the selection)
    const struct pids_stack *stack;
    unsigned long long tics0 = 0;
    unsigned long long tics1 = 0;
    uint16_t counter = 0;
    uint16_t least_top_cpu_time_idx = 0;
    double least_top_cpu_time=-1;
    

    while ((stack = procps_pids_get(info, PIDS_FETCH_TASKS_ONLY))) {
        // cpu time calc
        tics0 = PIDS_VAL(rel_ticks_all, ull_int, stack);
        int PID = PIDS_VAL(rel_pid, s_int, stack);
        struct timespec t0; 
        clock_gettime(CLOCK_MONOTONIC, &t0);

        usleep(10000);

        unsigned these[] = { (unsigned)PID };
        procps_pids_select(info2, these, sizeof(these)/sizeof(these[0]), PIDS_SELECT_PID);
        stack = procps_pids_get(info2, PIDS_FETCH_TASKS_ONLY);
        PID = PIDS_VAL(rel_pid, s_int, stack);
        tics1 = PIDS_VAL(rel_ticks_all, ull_int, stack);
        struct timespec t1; 
        clock_gettime(CLOCK_MONOTONIC, &t1);
        // calc cpu utilization
        double dt_wall = timespec_diff_sec(t1, t0);
        double hz = procps_hertz_get();
        double cpu_sec = (double)(tics1 - tics0) / hz;
        int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
        double util_pct = (cpu_sec / dt_wall) * 100.0 / ncpu;
        if(util_pct > least_top_cpu_time || counter < TOP_N){
            unsigned long rss_kib = PIDS_VAL(rel_mem, ul_int, stack);
            const char *cmd = PIDS_VAL(rel_cmd, str, stack);

            process_stats p;
            p.pid = PID;
            p.mem = (uint32_t)(rss_kib / 1024);
            p.cpu = (float)util_pct;
            strncpy(p.cmd, cmd, sizeof(p.cmd) - 1);
            p.cmd[sizeof(p.cmd) - 1] = '\0';

            insert_top_process(p);

        }        
    }

    procps_pids_unref(&info);
    procps_pids_unref(&info2);
    return out;
}

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
