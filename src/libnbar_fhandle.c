#include "include/libnbar.h"
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
        free(result);
        return NULL;
    } else {
        result->ar_file = fp;
    }

    // start scanning the header
    
    // if the file lacks a proper header, bail out
    if(fread(&header, sizeof(nbar_archive_header_t), 1, result->ar_file) != sizeof(nbar_archive_header_t)) {
        perror("nbar_fopen: error while reading archive header");
#       pragma region         
            free(result);
            fclose(fp);
#       pragma endregion
        return NULL;
    } else if (header._magic_left != 0x0617 || header._magic_right != 0x1033) {
        // if the Page and Left magic numbers do not match, the archive
        // fails to pass the NBAR specification. Bail out.
        errno = EILSEQ;
        fprintf(stderr, "nbar_fopen: invalid archive format");
        free(result);
        fclose(fp);
        return NULL; 
    }

    // check if the file matches the size the header specified
    struct stat ar_stat;
    if(!fstat(fileno(fp), &ar_stat)) {
        perror("nbar_fopen: fstat");
        free(result);
        fclose(fp);
        return NULL;
    }

    // in case the archive fails to
    // match the size it specified, bail out.
    if(ar_stat.st_size != (sizeof(nbar_archive_header_t) 
        + header.file_length_1 + header.file_length_2)) {
            errno = EILSEQ;
            fprintf(stderr, "nbar_fopen: tampered archive header");

            fclose(fp);
            free(result);

            return NULL;
    }

    // create two temporary files
    FILE *tmp_file_1 = tmpfile(), *tmp_file_2 = tmpfile();

    // bail out if any of the files fail to create
    if(tmp_file_1 == NULL || tmp_file_2 == NULL) {
        perror("nbar_fopen: tmpfile");
        fclose(fp);
        free(result);
        return NULL;
    }

    // locate the archive file pointer to the first file
    if(fseek(fp, sizeof(nbar_archive_header_t), SEEK_SET)) {
        perror("nbar_fopen: fseek");
        fclose(fp);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        free(result);
        return NULL;
    }
    
    // allocate a buffer for the first file
    size_t *tmp_buffer_1;
    if((tmp_buffer_1 = calloc(header.file_length_1, 1)) == NULL) {
        perror("nbar_fopen: calloc");
        fclose(fp);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        free(result);
        return NULL;
    }

    // load the first file.
    if(fread(tmp_buffer_1, header.file_length_1, 1, fp) != 1) {
        perror("nbar_fopen: fread");
        free(tmp_buffer_1);
        free(result);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        fclose(fp);
        return NULL;
    }

    // dump the first file into the temporary file
    if(fwrite(tmp_buffer_1, header.file_length_1, 1, tmp_file_1) != 1) {
        perror("nbar_fopen: fwrite");
        free(tmp_buffer_1);
        free(result);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        fclose(fp);
        return NULL;
    }

    size_t *tmp_buffer_2;
    if((tmp_buffer_2 = calloc(header.file_length_2, 1)) == NULL) {
        perror("nbar_fopen: calloc");
        free(tmp_buffer_1);
        free(result);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        fclose(fp);
        return NULL;
    }

    // load the second file
    if(fread(tmp_buffer_2, header.file_length_2, 1, fp) != 1) {
        perror("nbar_fopen: fread");
        free(tmp_buffer_1);
        free(tmp_buffer_2);
        free(result);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        fclose(fp);
   
        return NULL;
    }

    // dump the second file into the temporary file
    if(fwrite(tmp_buffer_2, header.file_length_2, 1, tmp_file_2) != 1) {
        perror("nbar_fopen: fwrite");

        free(tmp_buffer_1);
        free(tmp_buffer_2);
        free(result);
        fclose(tmp_file_1);
        fclose(tmp_file_2);
        fclose(fp);

        return NULL;
    }

    // construct the return value
    result->ar_content_1 =  tmp_file_1;
    result->ar_content_2 =  tmp_file_2;
    result->ar_file = fp;
    result->header = header;

    return result;
}