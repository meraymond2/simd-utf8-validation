#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "branchy.h"
#include "fsm.h"
#include "lookup.h"
#include "lookup256.h"
#include "lookup512-fake.h"

int main(int argc, char **argv) {
    char *filename = argv[1];
    struct stat sb;
    stat(filename, &sb);
    size_t filesize = sb.st_size;
    char unsigned *buf = malloc(filesize);
    FILE *input = fopen(filename, "r");
    fread(buf, 1, filesize, input);

    bool is_valid;
    clock_t start;
    clock_t end;
    float seconds;

    start = clock();
    is_valid = branchy_validate(buf, filesize);
    end = clock();
    seconds = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Branchy: ");
    printf("is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf\n", seconds);

    start = clock();
    is_valid = fsm_validate(buf, filesize);
    end = clock();
    seconds = (float) (end - start) / CLOCKS_PER_SEC;
    printf("FSM: ");
    printf("is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf\n", seconds);

    start = clock();
    is_valid = lookup_validate(buf, filesize);
    end = clock();
    seconds = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Lookup: ");
    printf("is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf\n", seconds);

    start = clock();
    is_valid = lookup256_validate(buf, filesize);
    end = clock();
    seconds = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Lookup 256: ");
    printf("is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf\n", seconds);

    start = clock();
    is_valid = lookup512_fake_validate(buf, filesize);
    end = clock();
    seconds = (float) (end - start) / CLOCKS_PER_SEC;
    printf("Lookup 512: ");
    printf("is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf\n", seconds);
    return 0;
}
