#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "inputData.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6

#define PACK_SIZE 5
#define ONE_BYTE 8

unsigned int getBit(unsigned int pixel, int bit) {
    return (pixel >> bit) & 0b00000001;
}

void ebcBlock_pack(Data *data) {
    for (int row = 0; row < data->height; row += 3) {
        int sum = 0;
        int count = 0;
        for (int i = 0; i < data->height; i += 3) {
            for (int j = 0; j < data->width; j += 3) {
                for (int row = i; row < i + 3 && row < data->height; row++) {
                    for (int column = j; column < j + 3 && column < data->width; column++) {
                        sum += data->unpackedData[row][column];
                        count++;
                    }
                }
                data->packedData[data->packedHeight][data->packedWidth] = (double)sum / count + 0.5;
                // printf("sum = %i, count = %i, avg = %i", sum, count, data->packedData[data->packedHeight][data->packedWidth]);
                printf("[%d][%d] = %i", data->packedHeight, data->packedWidth, data->packedData[data->packedHeight][data->packedWidth]);
                data->packedWidth++;
                if (data->packedWidth == data->width) {
                    data->packedWidth = 0;
                    data->packedHeight++;
                }
                // reset the sum and count
                sum = 0;
                count = 0;
            }
        }
    }
}

void ebcBlock_unpack(Data *data) {
    unsigned int **tmpData = (unsigned int **)malloc(data->height * sizeof(unsigned int *));
    unsigned int *tmpDataBlock = (unsigned int *)malloc(data->height * data->width * sizeof(unsigned int));
    for (int row = 0; row < data->height; row++) {
        tmpData[row] = tmpDataBlock + row * data->width;
    }
    int tmp_height = 0;
    int tmp_width = 0;
    for (int i = 0; i < data->height; i += 3) {
        for (int j = 0; j < data->width; j += 3) {
            for (int row = i; row < i + 3 && row < data->height; row++) {
                for (int column = j; column < j + 3 && column < data->width; column++) {
                    printf("[%d][%d] = %i ", row, column, data->unpackedData[tmp_height][tmp_width]);
                    tmpData[row][column] = data->unpackedData[tmp_height][tmp_width];
                }
            }
            tmp_width++;
            if (tmp_width == data->width) {
                tmp_width = 0;
                tmp_height++;
            }
        }
    }
    // now uncompressed ebu data is in tmpData
    free(data->unpackedData);
    free(data->unpackedDataBlock);
    data->unpackedData = tmpData;
    data->unpackedDataBlock = tmpDataBlock;
    // now uncompressed ebu data is in data->unpackedData
}

void packData(Data *data, unsigned int *buffer, int *bit_in_buffer, int row, int column) {
    // loop through each bit in the pixel
    for (int bit = 0; bit < PACK_SIZE; bit++) {
        // place the bit in the buffer
        *buffer |= getBit(data->packedData[row][column], PACK_SIZE - bit - 1) << (ONE_BYTE - *bit_in_buffer - 1);
        (*bit_in_buffer)++;

        // if the buffer is full, save it to the packed data
        if (*bit_in_buffer == ONE_BYTE) {
            data->packedData[data->packedHeight][data->packedWidth] = *buffer;

            // update the size of the packed data
            data->packedWidth++;
            if (data->packedWidth == data->width) {
                data->packedWidth = 0;
                data->packedHeight++;
            }

            // reset the buffer
            *buffer = 0;
            *bit_in_buffer = 0;
        }
    }
}

int unpackData(Data *data, unsigned int *buffer, int *bit_in_buffer, int row, int column, int *end, int *tmpHeight, int *tmpWidth) {
    // loop through each bit in the pixel
    for (int bit = 0; bit < ONE_BYTE; bit++) {
        // place the bit in the buffer
        *buffer |= getBit(data->packedData[row][column], ONE_BYTE - bit - 1) << (ONE_BYTE - *bit_in_buffer - 1);
        (*bit_in_buffer)++;

        // if the buffer is full, save it to the unpacked data
        if (*bit_in_buffer == PACK_SIZE && !*end) {
            data->unpackedData[*tmpHeight][*tmpWidth] = *buffer >> (ONE_BYTE - PACK_SIZE);

            // update the size of the unpacked data
            (*tmpWidth)++;
            if (*tmpWidth == data->width) {
                *tmpWidth = 0;
                (*tmpHeight)++;
            }
            if (*tmpHeight == data->packedHeight && *tmpWidth == data->packedWidth) {
                *end = 1;
            }

            // reset the buffer
            *buffer = 0;
            *bit_in_buffer = 0;
        }
    }

    return SUCCESS;
}
