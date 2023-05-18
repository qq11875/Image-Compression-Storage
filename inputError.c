
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "data2.h"
#include "pack-unpack.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define MAGIC_NUMBER_EB 0x6265
#define MAGIC_NUMBER_EC 0x6365
#define MAGIC_NUMBER_EU 0x7565
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAX_PIXEL_VALUE 31
#define MIN_PIXEL_VALUE 0

// function to check if the value is within the valid range
int isBadDataValue(unsigned int value) {
    if (value > MAX_PIXEL_VALUE || value < MIN_PIXEL_VALUE) {
        return BAD_DATA;
    }
    return SUCCESS;
}

// function to check if there is too much data in the file
int isDataTooMuch(unsigned int **array, FILE *inputFile) {
    if (fread(array, 1, 1, inputFile)) {
        return BAD_DATA;
    }
    return SUCCESS;
}

// function to check if magic number is correct
int isBadMagicNumber(unsigned char magicNumber[2], int correctMagicNumber) {
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    if (*magicNumberValue != correctMagicNumber) {
        return BAD_MAGIC_NUMBER;
    }

    return SUCCESS;
}

// function to check if dimensions are correct
int isBadDimension(Data *data, int check) {
    // check if we got 2 values, which is height and width
    if (check != 2) {
        return BAD_DIM;
    }

    // check if dimensions are within the valid range
    if (data->height < MIN_DIMENSION || data->width < MIN_DIMENSION || data->height > MAX_DIMENSION || data->width > MAX_DIMENSION) {
        return BAD_DIM;
    }

    return SUCCESS;
}

// function to check if the file opened successfully
// and the magic number is correct
// and the dimensions are correct
int isBadInput(Data *data, FILE *inputFile, int correctMagicNumber) {
    // check file opened successfully
    if (inputFile == NULL) {
        return BAD_FILE;
    }

    // get first 2 characters which should be magic number
    unsigned char magicNumber[2];
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);

    // check magic number
    if (isBadMagicNumber(magicNumber, correctMagicNumber)) {
        fclose(inputFile);
        return BAD_MAGIC_NUMBER;
    }

    // scan for the dimensions
    int check = fscanf(inputFile, "%d %d", &data->height, &data->width);

    // check dimensions
    if (isBadDimension(data, check)) {
        fclose(inputFile);
        return BAD_DIM;
    }

    return SUCCESS;
}