#ifndef _DEBUG_H
#define _DEBUG_H

#include "types.h"
#include <stdio.h>

// Useful debug functions

void m3_dbg_print_vec(const char* label, m3_vec vec) {
    printf("%s: (%d, %d, %d)\n", label, vec.x, vec.y, vec.z);
}

void m3_dbg_print_quat(const char* label, m3_quat quat) {
    printf("%s: (%02f, %02f, %02f, %02f)\n", label, quat.w / 127.0, quat.x / 127.0, quat.y / 127.0, quat.z / 127.0);
}

#endif