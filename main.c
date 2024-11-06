#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "branchy.h"

const char *filename = "/home/michael/dev/jpp/citylots.json";

int main(void) {
    struct stat sb;
    stat(filename, &sb);
    size_t filesize = sb.st_size;
    char *buf = malloc(filesize);
    FILE *input = fopen(filename, "r");
    fread(buf, 1, filesize, input);

    clock_t start = clock();
    bool is_valid = branchy_validate(buf, filesize);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    printf("Is valid: %s\n", is_valid ? "true" : "false");
    printf("Took: %.8lf", seconds);


    return 0;
}
