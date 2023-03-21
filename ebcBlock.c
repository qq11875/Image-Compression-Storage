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
    data->format[1] = 'u';

    int check = inputData_EU(data);
    if (check) {
        freeData(data);
        free(data);
        return check;
    }

    data->packedData = (unsigned int **)malloc(data->height * sizeof(unsigned int *));
    data->packedDataBlock = (unsigned int *)malloc(data->height * data->width * sizeof(unsigned int));

    // check if memory was allocated successfully
    if (!data->packedData || !data->packedDataBlock) {
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }

    for (int row = 0; row < data->height; row++) {
        data->packedData[row] = data->packedDataBlock + row * data->width;
    }

    data->packedHeight = 0;
    data->packedWidth = 0;
    ebcBlock_pack(data);

    unsigned int buffer = 0;
    int bit_in_buffer = 0;
    int end = 0;
    int heightLimit = data->packedHeight;
    int widthLimit = data->packedWidth;
    data->packedHeight = 0;
    data->packedWidth = 0;
    for (int row = 0; row < data->height && !end; row++) {
        for (int column = 0; column < data->width && !end; column++) {
            if (row == heightLimit && column == widthLimit) {
                end = 1;
                break;
            }
            packData(data, &buffer, &bit_in_buffer, row, column);
        }
    }

    // check if there is bit left in the buffer
    // if so, store it in the image data
    if (bit_in_buffer > 0) {
        data->packedData[data->packedHeight][data->packedWidth] = buffer;

        data->packedWidth++;
        if (data->packedWidth == data->width) {
            data->packedWidth = 0;
            data->packedHeight++;
        }
    }

    check = outputFile_EC(data, argv[2]);
    if (!check) {
        printf("CONVERTED\n");
    }

    printf("Height: %d Width: %d\n", data->packedHeight, data->packedWidth);

    freeData(data);
    free(data);
    return check;
}