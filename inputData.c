
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
#define MAGIC_NUMBER_EB 0x6265
#define MAGIC_NUMBER_EC 0x6365
#define MAGIC_NUMBER_EU 0x7565
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define MAX_PIXEL_VALUE 31
#define MIN_PIXEL_VALUE 0

// function to free memory allocated in Data struct
void freeData(Data *data) {
    free(data->unpackedData);
    free(data->unpackedDataBlock);
}

void freeMalloc(unsigned int **malloc1, unsigned int *malloc2) {
    if (malloc1) {
        free(malloc1);
    }
    if (malloc2) {
        free(malloc2);
    }
}

// function to check if the value is within the valid range
int checkDataValue(Data *data, unsigned int value) {
    if (value > MAX_PIXEL_VALUE || value < MIN_PIXEL_VALUE) {
        printf("ERROR: Bad Data (%s)\n", data->fileName);
        return BAD_DATA;
    }
    return SUCCESS;
}

// function to check if there is too much data in the file
int checkDataTooMuch(Data *data, FILE *inputFile) {
    if (data->format[1] == 'b') {
        if (!feof(inputFile)) {
            printf("ERROR: Bad Data (%s)\n", data->fileName);
            return BAD_DATA;
        }
    } else {
        if (fread(&data->unpackedData[0][0], 1, 1, inputFile)) {
            printf("ERROR: Bad Data (%s)\n", data->fileName);
            return BAD_DATA;
        }
    }
    return SUCCESS;
}

// function to check if magic number is correct
int magicNumberCheck(Data *data, unsigned char magicNumber[2], int correctMagicNumber) {
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    if (*magicNumberValue != correctMagicNumber) {
        printf("ERROR: Bad Magic Number (%s)\n", data->fileName);
        return BAD_MAGIC_NUMBER;
    }

    return SUCCESS;
}

// function to check if dimensions are correct
int dimensionCheck(Data *data, int check) {
    // check if we got 2 values, which is height and width
    if (check != 2) {
        printf("ERROR: Bad Dimensions (%s)\n", data->fileName);
        return BAD_DIM;
    }

    // check if dimensions are within the valid range
    if (data->height < MIN_DIMENSION || data->width < MIN_DIMENSION || data->height > MAX_DIMENSION || data->width > MAX_DIMENSION) {
        printf("ERROR: Bad Dimensions (%s)\n", data->fileName);
        return BAD_DIM;
    }

    return SUCCESS;
}

// function to check if the file opened successfully
// and the magic number is correct
// and the dimensions are correct
int inputCheck(Data *data, FILE *inputFile, int correctMagicNumber) {
    // check file opened successfully
    if (inputFile == NULL) {
        printf("ERROR: Bad File Name (%s)\n", data->fileName);
        return BAD_FILE;
    }

    // get first 2 characters which should be magic number
    unsigned char magicNumber[2];
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);

    // check magic number
    if (magicNumberCheck(data, magicNumber, correctMagicNumber)) {
        fclose(inputFile);
        return BAD_MAGIC_NUMBER;
    }

    // scan for the dimensions
    int check = fscanf(inputFile, "%d %d", &data->height, &data->width);

    // check dimensions
    if (dimensionCheck(data, check)) {
        fclose(inputFile);
        return BAD_DIM;
    }

    return SUCCESS;
}

// function to read in the data from the ebu file
int inputData_EU(Data *data) {
    // open the input file in read binary mode
    FILE *inputFile = fopen(data->fileName, "rb");

    // check if the file opened successfully
    // and the magic number is correct
    // and the dimensions are correct
    // if not, return the error code
    int check = inputCheck(data, inputFile, MAGIC_NUMBER_EU);
    if (check) {
        return check;
    }

    // allocate memory for the image data
    data->unpackedData = (unsigned int **)malloc(data->height * sizeof(unsigned int *));
    data->unpackedDataBlock = (unsigned int *)malloc(data->height * data->width * sizeof(unsigned int));

    // check if memory was allocated successfully
    if (!data->unpackedData || !data->unpackedDataBlock) {
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }

    // set the pointers to the correct location
    for (int row = 0; row < data->height; row++) {
        data->unpackedData[row] = data->unpackedDataBlock + row * data->width;
    }

    // there is a new line character after the dimensions
    // it is not part of the image data
    // so we need to read it in
    // but it will be overwritten by the first pixel value
    fread(&data->unpackedData[0][0], 1, 1, inputFile);

    // read in each grey value from the file
    for (int row = 0; row < data->height; row++) {
        for (int column = 0; column < data->width; column++) {
            check = fread(&data->unpackedData[row][column], 1, 1, inputFile);

            // validate that we have captured 1 pixel value
            if (check != 1) {
                fclose(inputFile);
                printf("ERROR: Bad Data (%s)\n", data->fileName);
                return BAD_DATA;
            }

            // check if the pixel value is within the valid range
            if (checkDataValue(data, data->unpackedData[row][column])) {
                fclose(inputFile);
                return BAD_DATA;
            }

            // check if there is too much data in the file
            if (row == data->height - 1 && column == data->width - 1) {  // data too much
                if (checkDataTooMuch(data, inputFile)) {
                    fclose(inputFile);
                    return BAD_DATA;
                }
            }
        }
    }

    fclose(inputFile);

    return SUCCESS;
}

// function to read in the data from the ebc file
int inputData_EC(Data *data) {
    // open the input file in read binary mode
    FILE *inputFile = fopen(data->fileName, "rb");

    // check if the file opened successfully
    // and the magic number is correct
    // and the dimensions are correct
    // if not, return the error code
    int check = inputCheck(data, inputFile, MAGIC_NUMBER_EC);
    if (check) {
        return check;
    }

    // allocate memory for the image data
    data->packedData = (unsigned int **)malloc(data->height * sizeof(unsigned int *));
    data->packedDataBlock = (unsigned int *)malloc(data->height * data->width * sizeof(unsigned int));

    // check if memory was allocated successfully
    if (!data->packedData || !data->packedDataBlock) {
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }

    // set the pointers to the correct location
    for (int row = 0; row < data->height; row++) {
        data->packedData[row] = data->packedDataBlock + row * data->width;
    }

    // there is a new line character after the dimensions
    // it is not part of the image data
    // so we need to read it in
    // but it will be overwritten by the first pixel value
    fread(&data->packedData[0][0], 1, 1, inputFile);

    data->packedHeight = 0;
    data->packedWidth = 0;
    int end = 0;

    // read in each grey value from the file
    for (int row = 0; row < data->height && !end; row++) {
        for (int column = 0; column < data->width && !end; column++) {
            check = fread(&data->packedData[row][column], 1, 1, inputFile);

            if (check != 1) {
                data->packedHeight = row;
                data->packedWidth = column;
                fclose(inputFile);
                return SUCCESS;
            }

            data->packedWidth++;
            if (data->packedWidth == data->width) {
                data->packedWidth = 0;
                data->packedHeight++;
            }
        }
    }

    fclose(inputFile);

    return SUCCESS;
}
