/**
 * @file mcrypt.c
 * @author Isaiah Smith
 * @brief Encrypt or decrypt a file using a binary key
 */

#include "KStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Read the key from a binary file
 * @param path The key file name
 * @param key Where to store the key
 * @return EXIT_SUCCESS if we got the key, EXIT_FAILURE otherwise
 */
static int read_key(const char *path, unsigned long *key) {
    FILE *key_file = fopen(path, "rb");
    if (key_file == NULL) {
        perror(path);
        return EXIT_FAILURE;
    }

    // read the whole key as one item
    size_t count = fread(key, sizeof *key, 1, key_file);
    fclose(key_file);

    if (count != 1) {
        fprintf(stderr, "mcrypt: could not read key from %s\n", path);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


/**
 * @brief Translate an input file into a binary output file
 * @param stream The keystream to use
 * @param in_path The input file name
 * @param out_path The output file name
 * @return EXIT_SUCCESS if the file was written, EXIT_FAILURE otherwise
 */
static int translate_file(KStream *stream, const char *in_path,
                          const char *out_path) {
    FILE *in_file = fopen(in_path, "rb");
    if (in_file == NULL) {
        perror(in_path);
        return EXIT_FAILURE;
    }

    FILE *out_file = fopen(out_path, "wb");
    if (out_file == NULL) {
        perror(out_path);
        fclose(in_file);
        return EXIT_FAILURE;
    }

    byte input[4096];
    byte output[4096];
    size_t count;
    int status = EXIT_SUCCESS;

    // use the same stream for every chunk
    while ((count = fread(input, 1, sizeof input, in_file)) > 0) {
        ks_translate(stream, input, output, count);

        if (fwrite(output, 1, count, out_file) != count) {
            perror(out_path);
            status = EXIT_FAILURE;
            break;
        }
    }

    // a zero-byte read can mean EOF or a read error
    if (ferror(in_file)) {
        perror(in_path);
        status = EXIT_FAILURE;
    }

    fclose(in_file);

    return status;
}


/**
 * @brief Set up the keystream and translate the input file
 * @param argc How many command-line arguments we got
 * @param argv The command-line arguments
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: mcrypt key-file in-file [ out-file | - ]\n");
        return EXIT_FAILURE;
    }

    unsigned long key;
    if (read_key(argv[ 1 ], &key) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // make one stream to use for the whole input file
    KStream *stream = ks_create(key);
    if (stream == NULL) {
        fprintf(stderr, "mcrypt: could not create keystream\n");
        return EXIT_FAILURE;
    }

        int status = translate_file(stream, argv[ 2 ], argv[ 3 ]);
    ks_destroy(stream);
    return status;
}