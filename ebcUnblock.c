#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "inputData.h"
#include "outputFile.h"
#include "pack-unpack.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MALLOC 5

int main(int argc, char **argv) {
    // validate that user has enter 2 arguments (plus the executable name)
    // check arg count

    if (argc == 1) {
        printf("Usage: ebu2ebc file1 file2");
        return SUCCESS;
    }
    if (argc != 3) {
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }

    Data *data = malloc(sizeof(Data));

    data->fileName = argv[1];
    data->format[0] = 'e';
    data->format[1] = 'c';

    inputData_EC(data);
    // now all data in packedData
    // and packedHeight and packedWidth are set to be the height and width of the packed data

    data->unpackedData = (unsigned int **)malloc(data->height * sizeof(unsigned int *));
    data->unpackedDataBlock = (unsigned int *)malloc(data->height * data->width * sizeof(unsigned int));

    // check if memory was allocated successfully
    if (!data->unpackedData || !data->unpackedDataBlock) {
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }

    for (int row = 0; row < data->height; row++) {
        data->unpackedData[row] = data->unpackedDataBlock + row * data->width;
    }

    unsigned int buffer = 0;
    int bit_in_buffer = 0;
    int end = 0;
    // now the unpackedData is empty
    // and packedHeight and packedWidth are set to be the height and width of the packed data
    int tmpHeight = 0;
    int tmpWidth = 0;
    for (int row = 0; row < data->height && !end; row++) {  // data should be put in unpackedData
        for (int column = 0; column < data->width && !end; column++) {
            unpackData(data, &buffer, &bit_in_buffer, row, column, &end, &tmpHeight, &tmpWidth);
        }
    }

    // now unpackedData has ebu data
    ebcBlock_unpack(data);

    // now unpackedData has unpacked ebu data

    int check = outputFile_EU(data, argv[2]);
    if (!check) {
        printf("CONVERTED\n");
    }

    printf("Height: %d Width: %d\n", data->height, data->width);

    freeData(data);
    free(data);
    return check;
}