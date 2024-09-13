#include "include/libnbar.h"
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

nbar_archive_t *nbar_fopen(char *filename, char *mode) {
    nbar_archive_t *result = calloc(sizeof(nbar_archive_t), 1);
    nbar_archive_header_t header;
    if (result == NULL) {
        perror("nbar_fopen");
        return NULL;
    }
    
    // open file
    FILE *fp;
    if((fp = fopen(filename, mode)) == NULL) {
        // bail out if file fails to open
        perror("nbar_fopen");
        return NULL;
    } else {
        result->ar_file = fp;
    }

    // start scanning the header
    
    // if the file lacks a proper header, bail out
    if(fread(&header, sizeof(nbar_archive_header_t), 1, result->ar_file) != sizeof(nbar_archive_header_t)) {
        perror("nbar_fopen: error while reading archive header");
        return NULL;
    } else if (header._magic_left != 0x0617 || header._magic_right != 0x1033) {
        // if the Page and Left magic numbers do not match, the archive
        // fails to pass the NBAR specification. Bail out.
        errno = EILSEQ;
        fprintf(stderr, "nbar_fopen: invalid archive format");
        return NULL; 
    }

    // check if the file matches the size the header specified
    struct stat ar_stat;
    if(!fstat(fileno(fp), &ar_stat)) {
        perror("nbar_fopen: fstat");
        return NULL;
    }

    // in case the archive fails to
    // match the size it specified, bail out.
    if(ar_stat.st_size != (sizeof(nbar_archive_header_t) 
        + header.file_length_1 + header.file_length_2)) {
            errno = EILSEQ;
            fprintf(stderr, "nbar_fopen: tampered archive header");
            return NULL;
    }

    // create two temporary files
    FILE *tmp_file_1 = tmpfile(), *tmp_file_2 = tmpfile();

    // bail out if any of the files fail to create
    if(tmp_file_1 == NULL || tmp_file_2 == NULL) {
        perror("nbar_fopen: tmpfile");
        return NULL;
    }

    // locate the archive file pointer to the first file
    if(fseek(fp, sizeof(nbar_archive_header_t), SEEK_SET) != sizeof(nbar_archive_header_t) - 1) {
        perror("nbar_fopen: fseek");
        return NULL;
    }
    
    // allocate a buffer for the first file
    size_t *tmp_buffer_1;
    if((tmp_buffer_1 = calloc(header.file_length_1, 1)) == NULL) {
        perror("nbar_fopen: calloc");
        return NULL;
    }

    // load the first file.
    if(fread(tmp_buffer_1, header.file_length_1, 1, fp) != header.file_length_1) {
        perror("nbar_fopen: fread");
        return NULL;
    }

    // dump the first file into the temporary file
    if(fwrite(tmp_buffer_1, header.file_length_1, 1, tmp_file_1) != header.file_length_1) {
        perror("nbar_fopen: fwrite");
        return NULL;
    }

    size_t *tmp_buffer_2;
    if((tmp_buffer_2 = calloc(header.file_length_2, 1)) == NULL) {
        perror("nbar_fopen: calloc");
        return NULL;
    }

    // load the second file
    if(fread(tmp_buffer_2, header.file_length_2, 1, fp) != header.file_length_2) {
        perror("nbar_fopen: fread");
        return NULL;
    }

    // dump the second file into the temporary file
    if(fwrite(tmp_buffer_2, header.file_length_2, 1, tmp_file_2) != header.file_length_2) {
        perror("nbar_fopen: fwrite");
        return NULL;
    }

    // construct the return value
    result->ar_content_1 =  tmp_file_1;
    result->ar_content_2 =  tmp_file_2;
    result->ar_file = fp;
    result->header = header;

    return result;
}