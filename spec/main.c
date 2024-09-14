#include <stdio.h>
#include <assert.h>
#include "../src/include/libnbar.h"

struct ltest {
    char    name[65];
    _Bool   (*runner)(void);
};

int main(void) {
    struct ltest tests[] = {};

    for (int i = 0; i < sizeof(tests) / sizeof(struct ltest); i++) {
        fprintf(stdout, "testing %s...", tests[i].name);
        if(tests[i].runner()) {
            fprintf(stdout, "test \"%s\" succeeded.", tests[i].name);
        } else {
            fprintf(stdout, "test \"%s\" failed.", tests[i].name);
        }
    }
    return 0;
}