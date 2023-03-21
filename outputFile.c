#include <stdio.h>

#include "data.h"

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

// function to write the data to a ebu file
int outputFile_EU(Data *data, char *fileName) {
    // open the output file in write binary mode
    FILE *outputFile = fopen(fileName, "wb");

    // validate that the file has been opened correctly
    if (!outputFile) {
        printf("ERROR: Bad File Name (%s)\n", fileName);
        return BAD_FILE;
    }

    // write the header data in one block
    fprintf(outputFile, "eu\n%d %d\n", data->height, data->width);

    // iterate though the array and print out pixel values
    for (int row = 0; row < data->height; row++) {
        for (int column = 0; column < data->width; column++) {
            fwrite(&data->unpackedData[row][column], 1, 1, outputFile);
        }
    }

    fclose(outputFile);

    return SUCCESS;
}

int outputFile_EC(Data *data, char *fileName) {
    // open the output file in write binary mode
    FILE *outputFile = fopen(fileName, "wb");

    // validate that the file has been opened correctly
    if (!outputFile) {
        printf("ERROR: Bad File Name (%s)\n", fileName);
        return BAD_FILE;
    }

    // write the header data in one block
    fprintf(outputFile, "ec\n%d %d\n", data->height, data->width);
    // printf("row = %d, column = %d\n", data->packedHeight, data->packedWidth);
    //  iterate though the array and print out pixel values
    for (int row = 0; row < data->height; row++) {
        for (int column = 0; column < data->width; column++) {
            // if we have reached the end of the data, close the file and exit
            if (row == data->packedHeight && column == data->packedWidth) {
                fclose(outputFile);
                return SUCCESS;
            }

            fwrite(&data->packedData[row][column], 1, 1, outputFile);
        }
    }

    fclose(outputFile);

    return SUCCESS;
}