#ifndef BASE64_H
#define BASE64_H

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Encodes binary data to base64 string
 * @param src Pointer to source binary data
 * @param src_len Length of source data in bytes
 * @param dst Pointer to destination buffer for base64 string
 * @param dst_size Size of destination buffer (must be at least base64_encode_len(src_len) + 1)
 * @return Number of bytes written to dst (excluding null terminator), or -1 on error
 */
int base64_encode(const uint8_t *src, size_t src_len, char *dst, size_t dst_size);

/**
 * @brief Decodes base64 string to binary data
 * @param src Pointer to base64 string (null-terminated)
 * @param dst Pointer to destination buffer for binary data
 * @param dst_size Size of destination buffer (must be at least base64_decode_len(strlen(src)))
 * @return Number of bytes written to dst, or -1 on error
 */
int base64_decode(const char *src, uint8_t *dst, size_t dst_size);

/**
 * @brief Calculates the required buffer size for base64 encoding
 * @param src_len Length of source binary data in bytes
 * @return Required size of destination buffer (including space for null terminator)
 */
size_t base64_encode_len(size_t src_len);

/**
 * @brief Calculates the maximum required buffer size for base64 decoding
 * @param src_len Length of base64 string in bytes (excluding null terminator)
 * @return Maximum size needed for destination buffer
 */
size_t base64_decode_len(size_t src_len);

#endif // BASE64_H
