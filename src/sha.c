// sha.c
#include "bit.h"
void bit_sha256(const char* file) {
    char cmd[512];
    mkdir(BIT_DIR, 0755);
    mkdir(".bit/fetch", 0755);
    sprintf(cmd, "sha256sum %s > %s", file, BIT_SHA);
    if(system(cmd) == 0) printf("[SHA] Saved to %s\n", BIT_SHA);
}

