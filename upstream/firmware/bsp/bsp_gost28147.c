#include <stdio.h>
#include <stdint.h>
#include <string.h>

static uint32_t subkeys[8];

// E-TEST S-Box (CryptoPro Parameter Set A / RFC 4357)
static const uint8_t sbox[8][16] = {
    {4,10,9,2,13,8,0,14,6,11,1,12,7,15,5,3},
    {14,11,4,12,6,13,15,10,2,3,8,1,0,7,5,9},
    {5,8,1,13,10,3,4,2,14,15,12,7,6,0,9,11},
    {7,13,10,1,0,8,9,15,14,4,6,12,11,2,5,3},
    {6,12,7,1,5,15,13,8,4,10,9,14,0,3,11,2},
    {4,11,10,0,7,2,1,13,3,6,8,5,9,12,15,14},
    {13,11,4,1,3,15,5,9,0,10,14,7,6,8,2,12},
    {1,15,13,0,5,7,10,4,9,2,3,14,6,11,8,12}
};

static const int key_order_encrypt[32] = {
    0,1,2,3,4,5,6,7, 0,1,2,3,4,5,6,7, 0,1,2,3,4,5,6,7, 7,6,5,4,3,2,1,0
};

static const int key_order_decrypt[32] = {
    0,1,2,3,4,5,6,7, 7,6,5,4,3,2,1,0, 7,6,5,4,3,2,1,0, 7,6,5,4,3,2,1,0
};

static uint32_t gost_f(uint32_t x, uint32_t k) {
    uint32_t sum = x + k;
    uint32_t out = 0;
    for(int i = 0; i < 8; i++) {
        uint8_t nibble = (sum >> (i * 4)) & 0xF;
        out |= ((uint32_t)sbox[i][nibble]) << (i * 4);
    }
    return (out << 11) | (out >> 21);
}

static void gost_process(uint8_t *block, const int *key_order, int is_decrypt) {
    // Unpack bytes into Little-Endian words
    uint32_t N1 = ((uint32_t)block[3] << 24) | ((uint32_t)block[2] << 16) | ((uint32_t)block[1] << 8) | block[0];
    uint32_t N2 = ((uint32_t)block[7] << 24) | ((uint32_t)block[6] << 16) | ((uint32_t)block[5] << 8) | block[4];

    for(int i = 0; i < 32; i++) {
        uint32_t temp = gost_f(N1, subkeys[key_order[i]]) ^ N2;
        if (i < 31) {
            N2 = N1;
            N1 = temp;
        } else {
            N2 = temp; // No final round swap in core Feistel
        }
    }

    // CyberChef compatibility: Decryption expects N1 then N2 (swapped output)
    uint32_t out_L = is_decrypt ? N1 : N2;
    uint32_t out_R = is_decrypt ? N2 : N1;

    block[0] = out_L & 0xFF;        block[1] = (out_L >> 8) & 0xFF; 
    block[2] = (out_L >> 16) & 0xFF; block[3] = (out_L >> 24) & 0xFF;
    block[4] = out_R & 0xFF;        block[5] = (out_R >> 8) & 0xFF; 
    block[6] = (out_R >> 16) & 0xFF; block[7] = (out_R >> 24) & 0xFF;
}

// Set the key for GOST 28147 cipher (Little-Endian)
void gost28147_set_key(const uint8_t *key) {
    for(int i = 0; i < 8; i++) {
        subkeys[i] = ((uint32_t)key[i*4 + 3] << 24) | 
                     ((uint32_t)key[i*4 + 2] << 16) | 
                     ((uint32_t)key[i*4 + 1] << 8)  | 
                     key[i*4];
    }
}

// Encrypt a 64-bit block in place
void gost28147_encrypt(uint8_t *block) {
    gost_process(block, key_order_encrypt, 0);
}

// Decrypt a 64-bit block in place
void gost28147_decrypt(uint8_t *block) {
    gost_process(block, key_order_decrypt, 1);
}