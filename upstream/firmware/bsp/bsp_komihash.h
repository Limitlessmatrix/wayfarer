#ifndef BSP_KOMIHASH_H
#define BSP_KOMIHASH_H

#include <stdint.h>
#include <stddef.h>

// KOMIHASH is a fast 64-bit hash function
uint64_t komihash(const void *data, size_t len, uint64_t seed);

#endif // BSP_KOMIHASH_H