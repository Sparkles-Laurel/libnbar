// libnbar.h - Single file library for Non-Binary Archives
#ifndef LIBNBAR_H
#   define LIBNBAR_H

#   include <stdio.h>
#   include <stdint.h>
#   include <stdalign.h>
#   include <stddef.h>
#   include <uchar.h>
#   include <string.h>
#   include <errno.h>
#   include <unistd.h>
#   include <sys/stat.h>
#   include <sodium.h>
#   include "util/test_platform.h"

#   define NBAR_MAGIC_NUMBER_LEFT    0x0617
#   define NBAR_MAGIC_NUMBER_RIGHT   0x1033

#   define NBAR_SELECT_1ST 0
#   define NBAR_SELECT_2ND 1

    typedef struct __attribute__((packed)) {
        uint16_t     _magic_left; // must be equal to 0x0617
        uint8_t      file_name_length_1;
        uint8_t      file_name_length_2;
        char         file_name_1[256];
        char         file_name_2[256];
        char         file_checksum_1[crypto_hash_sha512_BYTES];
        char         file_checksum_2[crypto_hash_sha512_BYTES];
        uint64_t     file_length_1;
        uint64_t     file_length_2;
        uint16_t     _magic_right; // must be equal to 0x1033
    } nbar_archive_header_t;

#   define NBAR_ARCHIVE_HEADER_SIZE(ar) \
        (sizeof(nbar_archive_header_t) - (2 * sizeof(char)) + (ar).file_name_length_1 + (ar).file_name_length_2 ) 

    typedef struct {
        nbar_archive_header_t   header;
        FILE                   *ar_content_1;
        FILE                   *ar_content_2;
        FILE                   *ar_file;
    } nbar_archive_t;

    nbar_archive_t *nbar_fopen (char *filename, char *mode);
    void            nbar_fclose(nbar_archive_t *handle);
#endif