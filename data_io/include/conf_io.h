#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#pragma once

#define MAX_LINE 256
#define MAX_KEY  64
#define MAX_VAL  192

// Trim leading and trailing spaces
char *trim(char *str);

// Read conf file and print key-value pairs
void read_conf(const char *filename);

