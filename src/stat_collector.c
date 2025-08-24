#include <stdio.h>
#include "conf_io.h"
#include "stats_io.h"



int main() {
    printf("Hello, World!\n");
    const char *conf_file = "../conf/defconf.conf";
    read_conf(conf_file);   
    process_stats stats = read_stats();
    for (int i = 0; i < TOP_N; i++) {
        write_stats(top_processes[i]);
    }
    return 0; 
}




