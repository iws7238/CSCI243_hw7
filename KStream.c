#include "KStream.h"

#include <stdlib.h>

struct KStream {
    byte S[256];
    size_t i;
    size_t j;
};


static byte next_byte( KStream *stream) {
    // Advance i and j, % 256 to wrap around after 255
    stream->i = ( stream->i + 1) % 256;
    stream->j = ( stream->j + stream->S[stream->i]) % 256;

    // Preserve i value when we write over it
    byte temp = stream->S[stream->i];
    stream->S[stream->i] = stream->S[stream->j];
    stream->S[stream->j] = temp;

    size_t B = ( stream->S[stream->i] + stream->S[stream->j]) % 256;
    return stream->S[B];
}

KStream *ks_create(unsigned long key) {
    // Allocate memory for stream
    KStream *stream = malloc(sizeof *stream);
    if (stream == NULL) {
        return NULL;
    }
    // Fill s with 0 through 255
    for (size_t i = 0; i < 256; i++) {
        stream->S[i] = (byte)i;
    }

    // read the key as bytes in the same order they were read from the file
    const byte *key_bytes = ( const byte *)&key;

    // Shuffle S, cycling back to the first key byte every eight steps.
    size_t j = 0;
    for (size_t i = 0; i < 256; i++) {
        j = (j + stream->S[i] + key_bytes[ i % 8 ]) % 256;

        byte temp = stream->S[i];
        stream->S[i] = stream->S[j];
        stream->S[j] = temp;
    }

    // Start generation at zero and discard the first 1024 bytes
    stream->i = 0;
    stream->j = 0;
    for  (size_t count = 0; count < 1024; count++ ) {
        next_byte(stream);
    }

    return stream;
}


void ks_translate(KStream *stream, const byte *input, byte *output,size_t count) {
    // Use one keystream byte for each input byte
    for (size_t i = 0; i < count; i++) {
        output[i] = input[i] ^ next_byte(stream);
    }
}


void ks_destroy(KStream *stream) {
    free(stream);
}