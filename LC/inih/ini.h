#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef int (*ini_handler)(void* user, const char* section,
                           const char* name, const char* value);

int ini_parse(const char* filename, ini_handler handler, void* user);

#ifdef __cplusplus
}
#endif
