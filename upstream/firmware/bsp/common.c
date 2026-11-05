#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "base64.h"
#include "bsp_komihash.h"
#include "config.h"

int g_seed = 0;
uint64_t signkey = 0;

void pseudo_random_init(int seed) {
    g_seed = seed;
}

int psuedo_random() {
    // Simple linear congruential generator (LCG) for demonstration
    g_seed = (g_seed * 1664525 + 1013904223) % 0xFFFFFFFF;
    return g_seed;
}

void sign_message(char* message, size_t msg_size) {
    char sig[13];
    if (signkey == 0) {
        char* id = config_get_id();
        signkey = ((uint32_t)id[0] << 0)  |
                  ((uint32_t)id[1] << 8)  |
                  ((uint32_t)id[2] << 16) |
                  ((uint32_t)id[3] << 24) |
                  ((uint64_t)id[4] << 32) |
                  ((uint64_t)id[5] << 40);
        //printf("Sign key initialized: %016llx\n", signkey);
    }

    if (msg_size == 0 || message == NULL) {
        return;
    }

    size_t len = strnlen(message, msg_size);
    if (len >= msg_size - 1) {
        return;
    }

    uint64_t hash = komihash(message, len, signkey);
    base64_encode((uint8_t*)&hash, sizeof(hash), sig, sizeof(sig));
    size_t append_len = 4;
    size_t available = msg_size - len - 1;
    if (append_len > available) {
        append_len = available;
    }
    strncat(message, sig, append_len);
}