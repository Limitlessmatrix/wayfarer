#ifndef BSP_GOST28147_H
#define BSP_GOST28147_H

#include <stdint.h>
#include <stddef.h>

#define GOST_BLOCK_SIZE 8
#define GOST_KEY_SIZE 32
#define GOST_HASH_SIZE 32

// Set the key for GOST 28147 cipher
void gost28147_set_key(const uint8_t *key);

// Encrypt a 64-bit block in place
void gost28147_encrypt(uint8_t *block);

// Decrypt a 64-bit block in place
void gost28147_decrypt(uint8_t *block);

#endif // BSP_GOST28147_H