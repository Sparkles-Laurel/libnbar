// libnbar.h - Single file library for Non-Binary Archives
#ifndef LIBNBAR_H
#   define LIBNBAR_H

#include <cstdint>
#include <cstdio>
#   include <stdint.h>
#   include <stdalign.h>
#   include <stddef.h>
#   include <uchar.h>

#   include "util/test_platform.h"

#   define MAGIC_NUMBER_LEFT    0x0617
#   define MAGIC_NUMBER_RIGHT   0x1033

    typedef struct __attribute__((packed)) {
        uint16_t     _magic_left; // must be equal to 0x0617
        uint8_t      file_name_length_1;
        uint8_t      file_name_length_2;
        char8_t    *file_name_1;
        char8_t    *file_name_2;
        uint64_t     file_length_1;
        uint64_t     file_length_2;
        uint16_t     _magic_right; // must be equal to 0x1033
    } nbar_archive_header_t;

#   define NBAR_ARCHIVE_HEADER_SIZE(ar) \
        (sizeof(nbar_archive_header_t) - (2 * sizeof(char8_t)) + (ar).file_name_length_1 + (ar).file_name_length_2 ) 

    typedef struct {
        nbar_archive_header_t   header;
        uint8_t                *file_block_1;
        uint8_t                *file_block_2;
        uint64_t                offset_block_1;
        uint64_t                offset_block_2;
        FILE                   *ar_file;
    } nbar_archive_t;

    nbar_archive_t *nbar_fopen (char *filename);
    void            nbar_fclose(nbar_archive_t *handle);
    size_t          nbar_fread (void *buf, size_t block_size, size_t block_count, nbar_archive_t *handle, uint8_t nth_file);
    size_t          nbar_fwrite(void *buf, size_t block_size, size_t block_count, nbar_archive_t *handle, uint8_t nth_file);
    uint64_t        nbar_ftell (nbar_archive_t *handle, uint8_t nth_file);
    FILE           *nbar_fopen_nth(nbar_archive_t *handle, uint8_t nth_file);
#endif