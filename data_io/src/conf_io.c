#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "conf_io.h"

// Trim leading and trailing spaces
char *trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}
// Check if directory exists and create it if it doesn't
int ensure_directory_exists(const char *path) {
    struct stat st = {0};
    
    // Check if directory exists
    if (stat(path, &st) == 0) {
        // Path exists, check if it's a directory
        if (S_ISDIR(st.st_mode)) {
            return 0; // Directory exists
        } else {
            printf("Error: %s exists but is not a directory\n", path);
            return -1;
        }
    }
    
    // Directory doesn't exist, try to create it
    if (mkdir(path, 0755) == 0) {
        printf("Created directory: %s\n", path);
        return 0;
    } else {
        printf("Failed to create directory %s: %s\n", path, strerror(errno));
        return -1;
    }
}
// Read conf file and print key-value pairs
void read_conf(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
    }
    printf("file opened\n");
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        printf("line %s \n",line);
        char *trimmed = trim(line);
         printf("trimmed line %s \n",trimmed);
        // Ignore comments and empty lines
        if (trimmed[0] == '#' || trimmed[0] == '\0')
            continue;

        char key[MAX_KEY], val[MAX_VAL];
        if (sscanf(trimmed, "%63[^=]=%191[^\n]", key, val) == 2) {
            char *k = trim(key);
            char *v = trim(val);
            if (strcmp(key, "LOGGING_DIR") == 0) {
                strncpy(LOGGING_DIR, v, 32);
                if (ensure_directory_exists(LOGGING_DIR) != 0) {
                    printf("Warning: Could not ensure LOGGING_DIR exists: %s\n", LOGGING_DIR);
                }
            }else if(strcmp(key, "DELAY_BETWEEN_CALLS") == 0){
                DELAY_BETWEEN_CALLS = atof(v);
            }
            printf("Key: '%s' | Value: '%s'\n", k, v);
        }
    }

    fclose(fp);
}

// int main() {
//     const char *conf_file = "config.conf";
//     read_conf(conf_file);    
//     printf("LOGGING_DIR %s REFRESH_RATE %f\n",LOGGING_DIR,REFRESH_RATE);
//     return 0;
// }
