#include "include/libnbar.h"
#include <stdlib.h>

nbar_archive_t *nbar_fopen(char *filename) {
    nbar_archive_t *result = malloc(sizeof(nbar_archive_t));
    nbar_archive_header_t header;
    if (result == NULL) return NULL;
    
    // start scanning the file header

}