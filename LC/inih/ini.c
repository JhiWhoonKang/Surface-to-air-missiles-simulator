#include "ini.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 200

static char* rstrip(char* s) {
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

int ini_parse(const char* filename, ini_handler handler, void* user) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;

    char line[MAX_LINE];
    char section[50] = "";
    while (fgets(line, MAX_LINE, file) != NULL) {
        char* start = line;
        while (*start && isspace((unsigned char)(*start))) start++;
        if (*start == ';' || *start == '#') continue;
        if (*start == '[') {
            char* end = strchr(start, ']');
            if (end) {
                *end = '\0';
                strncpy(section, start + 1, sizeof(section) - 1);
            }
        } else if (*start) {
            char* equals = strchr(start, '=');
            if (equals) {
                *equals = '\0';
                char* name = rstrip(start);
                char* value = equals + 1;
                value = rstrip(value);
                handler(user, section, name, value);
            }
        }
    }
    fclose(file);
    return 0;
}
