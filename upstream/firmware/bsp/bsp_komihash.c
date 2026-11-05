#include "bsp_komihash.h"

#define PRIME1 0x27d4eb2f165667c5ULL
#define PRIME2 0x9e3779b97f4a7c15ULL
#define PRIME3 0xc2b2ae3d27d4eb4fULL
#define PRIME4 0x165667b19e3779f9ULL
#define PRIME5 0x85ebca77c2b2ae63ULL

uint64_t komihash(const void *data, size_t len, uint64_t seed) {
    const uint8_t *p = (const uint8_t *)data;
    uint64_t h = seed ^ PRIME5;

    // Process 8-byte blocks
    size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        uint64_t k;
        // Assuming little-endian, as most systems
        k = (uint64_t)p[i] | ((uint64_t)p[i+1] << 8) | ((uint64_t)p[i+2] << 16) | ((uint64_t)p[i+3] << 24) |
            ((uint64_t)p[i+4] << 32) | ((uint64_t)p[i+5] << 40) | ((uint64_t)p[i+6] << 48) | ((uint64_t)p[i+7] << 56);
        h ^= k;
        h = (h * PRIME1) + PRIME2;
    }

    // Handle remaining bytes
    uint64_t tail = 0;
    size_t rem = len - i;
    for (size_t j = 0; j < rem; j++) {
        tail |= (uint64_t)p[i + j] << (j * 8);
    }
    h ^= tail;
    h = (h * PRIME1) + PRIME2;

    // Finalization
    h ^= len;
    h = (h * PRIME3) + PRIME4;
    h ^= (h >> 33);
    h = (h * PRIME1) + PRIME2;
    h ^= (h >> 29);
    h = (h * PRIME3) + PRIME4;
    h ^= (h >> 32);

    return h;
}