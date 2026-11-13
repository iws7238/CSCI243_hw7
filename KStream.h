/**
 * @file KStream.h
 * @author Isaiah Smith
 * @brief Interface for stream cipher
 */

#ifndef KSTREAM_H
#define KSTREAM_H

#include <stddef.h>

/** An unsigned byte of binary data */
typedef unsigned char byte;

/** Stream cipher state */
typedef struct KStream KStream;

/**
 * @brief Create and prime a keystream using a binary key
 * @param key The key value read by the caller from the binary key file
 * @return A new keystream, or NULL if allocation fails
 */
KStream *ks_create(unsigned long key);

/**
 * @brief Release the storage owned by a keystream
 * @param stream The keystream to destroy
 * @pre stream is NULL or was returned by ks_create and is still valid
 */
void ks_destroy(KStream *stream);

/**
 * @brief Encrypt or decrypt bytes using the next bytes of the keystream
 * @param stream The keystream to use and advance
 * @param input The caller-owned buffer containing the bytes to translate
 * @param output The caller-owned buffer that receives the translated bytes
 * @param count The number of bytes to translate
 * @pre stream points to a valid keystream
 * @pre input and output are separate buffers of at least count bytes each
 * @post output contains count translated bytes, and stream has advanced
 *       by count bytes
 */
void ks_translate(KStream *stream, const byte *input, byte *output,
                  size_t count);

#endif