#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "pack-unpack.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6

void reportError(int errorValue, const char *fileName) {
    switch (errorValue) {
        case BAD_FILE:
            printf("ERROR: Bad File (%s)\n", fileName);
            break;
        case BAD_MAGIC_NUMBER:
            printf("ERROR: Bad Magic Number (%s)\n", fileName);
            break;
        case BAD_DIM:
            printf("ERROR: Bad Dimension (%s)\n", fileName);
            break;
        case BAD_MALLOC:
            printf("ERROR: Bad Malloc (%s)\n", fileName);
            break;
        case BAD_DATA:
            printf("ERROR: Bad Data (%s)\n", fileName);
        default:
            break;
    }
}