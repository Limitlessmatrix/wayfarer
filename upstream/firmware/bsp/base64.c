#include "base64.h"
#include <string.h>
#include <stdio.h>

static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t base64_encode_len(size_t src_len) {
    return ((src_len + 2) / 3) * 4 + 1; // +1 for null terminator
}

size_t base64_decode_len(size_t src_len) {
    return (src_len * 3) / 4 + 1;
}

int base64_encode(const uint8_t *src, size_t src_len, char *dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0) {
        return -1;
    }

    size_t required_size = base64_encode_len(src_len);
    if (dst_size < required_size) {
        printf("base64 %d<%d (%d)\r\n", dst_size, required_size, src_len);
        return -1;
    }

    size_t i = 0;
    size_t j = 0;

    while (i < src_len) {
        uint8_t b1 = src[i++];
        uint8_t b2 = (i < src_len) ? src[i++] : 0;
        uint8_t b3 = (i < src_len) ? src[i++] : 0;

        uint32_t triple = (b1 << 16) | (b2 << 8) | b3;

        dst[j++] = base64_table[(triple >> 18) & 0x3F];
        dst[j++] = base64_table[(triple >> 12) & 0x3F];
        
        if (i - 1 < src_len) {
            dst[j++] = base64_table[(triple >> 6) & 0x3F];
        } else {
            dst[j++] = '=';
        }
        
        if (i < src_len) {
            dst[j++] = base64_table[triple & 0x3F];
        } else {
            dst[j++] = '=';
        }
    }

    dst[j] = '\0';
    return j;
}

int base64_decode(const char *src, uint8_t *dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0) {
        return -1;
    }

    size_t src_len = strlen(src);
    size_t required_size = base64_decode_len(src_len);
    if (dst_size < required_size) {
        return -1;
    }

    // Create reverse lookup table
    static uint8_t decode_table[256];
    static int decode_table_init = 0;
    
    if (!decode_table_init) {
        memset(decode_table, 0xFF, sizeof(decode_table));
        for (int i = 0; i < 64; i++) {
            decode_table[(uint8_t)base64_table[i]] = i;
        }
        decode_table_init = 1;
    }

    size_t i = 0;
    size_t j = 0;

    while (i < src_len && src[i] != '\0') {
        // Get 4 characters
        uint8_t c1 = src[i++];
        uint8_t c2 = (i < src_len) ? src[i++] : 0;
        uint8_t c3 = (i < src_len) ? src[i++] : 0;
        uint8_t c4 = (i < src_len) ? src[i++] : 0;

        // Decode
        uint8_t b1 = decode_table[c1];
        uint8_t b2 = decode_table[c2];
        uint8_t b3 = decode_table[c3];
        uint8_t b4 = decode_table[c4];

        // Check for invalid characters
        if (b1 == 0xFF || b2 == 0xFF) {
            return -1;
        }

        uint32_t triple = ((uint32_t)b1 << 18) | ((uint32_t)b2 << 12);

        if (c3 != '=' && c3 != '\0') {
            if (b3 == 0xFF) {
                return -1;
            }
            triple |= ((uint32_t)b3 << 6);
        } else {
            // Only 1 byte of output
            if (j + 1 > dst_size) {
                return -1;
            }
            dst[j++] = (triple >> 16) & 0xFF;
            break;
        }

        if (c4 != '=' && c4 != '\0') {
            if (b4 == 0xFF) {
                return -1;
            }
            triple |= b4;
        } else {
            // Only 2 bytes of output
            if (j + 2 > dst_size) {
                return -1;
            }
            dst[j++] = (triple >> 16) & 0xFF;
            dst[j++] = (triple >> 8) & 0xFF;
            break;
        }

        // All 3 bytes of output
        if (j + 3 > dst_size) {
            return -1;
        }
        dst[j++] = (triple >> 16) & 0xFF;
        dst[j++] = (triple >> 8) & 0xFF;
        dst[j++] = triple & 0xFF;
    }

    return j;
}
