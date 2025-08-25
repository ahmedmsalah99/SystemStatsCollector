#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "conf_io.h"
#include "stats_io.h"


#define APP_NAME "stat_collector"

// Try to locate config file
const char* find_conf_file() {
    static char path[64];

    // 1. Check environment override (MYAPP_CONF)
    const char *def_path = "../conf/defconf.conf";
    if (def_path && access(def_path, R_OK) == 0) {
        return def_path;
    }
    // 2. Linux/macOS system config
    snprintf(path, sizeof(path), "/etc/%s/deployconf.conf", APP_NAME);
    if (access(path, R_OK) == 0) return path;

    return NULL; // not found
}

int main() {
    const char *conf_file = find_conf_file();
    if (!conf_file) {
        fprintf(stderr, "Error: could not locate configuration file.\n");
        return 1;
    }
    read_conf(conf_file);  
    process_stats stats = read_stats();
    for (int i = 0; i < TOP_N; i++) {
        write_stats(top_processes[i]);
    }
    return 0; 
}