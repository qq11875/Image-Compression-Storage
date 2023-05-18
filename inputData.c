
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "memoryUtils.h"
#include "pack-unpack.h"
#include "reportError.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define MAGIC_NUMBER_ec 0x6365
#define MAGIC_NUMBER_EC 0x4345
#define MAGIC_NUMBER_E5 0x3545
#define MAGIC_NUMBER_E7 0x3745
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

// function to check if there is too much data in the file
int checkDataTooMuch(const char *fileName, ImageData *data, FILE *inputFile) {
    if (fread(&data->data[0][0], 1, 1, inputFile)) {
        return BAD_DATA;
    }

    return SUCCESS;
}

// function to check if magic number is correct
int magicNumberCheck(const char *fileName, ImageData *data, unsigned char magicNumber[2], int correctMagicNumber) {
    unsigned short *magicNumberValue = (unsigned short *)magicNumber;

    if (*magicNumberValue != correctMagicNumber) {
        return BAD_MAGIC_NUMBER;
    }

    return SUCCESS;
}

// function to check if dimensions are correct
int dimensionCheck(const char *fileName, ImageData *data, int check) {
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
int inputCheck(const char *fileName, ImageData *data, FILE *inputFile, int correctMagicNumber) {
    // check file opened successfully
    if (inputFile == NULL) {
        return BAD_FILE;
    }

    // get first 2 characters which should be magic number
    unsigned char magicNumber[2];
    magicNumber[0] = getc(inputFile);
    magicNumber[1] = getc(inputFile);

    // check magic number
    if (magicNumberCheck(fileName, data, magicNumber, correctMagicNumber)) {
        fclose(inputFile);
        return BAD_MAGIC_NUMBER;
    }

    // scan for the dimensions
    int check = fscanf(inputFile, "%d %d", &data->height, &data->width);

    // check dimensions
    if (dimensionCheck(fileName, data, check)) {
        fclose(inputFile);
        return BAD_DIM;
    }

    return SUCCESS;
}

ImageData *getImageDataFromEC(const char *fileName, int *errorCode) {
    FILE *inputFile = fopen(fileName, "rb");
    ImageData *packedImage = malloc(sizeof(ImageData));
    ImageData *unpackedImage = malloc(sizeof(ImageData));
    *errorCode = inputCheck(fileName, packedImage, inputFile, MAGIC_NUMBER_ec);
    if (*errorCode != SUCCESS) {
        reportError(*errorCode, fileName);
        freeImageData(packedImage);
        freeImageData(unpackedImage);
        return NULL;
    }
    unpackedImage->height = packedImage->height;
    unpackedImage->width = packedImage->width;

    allocate2DArrayMemory(packedImage, packedImage->height, packedImage->width);
    fread(&packedImage->data[0][0], 1, 1, inputFile);

    for (int row = 0, end = 0; row < packedImage->height && !end; row++) {
        for (int column = 0; column < packedImage->width && !end; column++) {
            if (fread(&packedImage->data[row][column], 1, 1, inputFile) != 1) {
                packedImage->height = row;
                packedImage->width = column;
                end = 1;
            }
        }
    }
    fclose(inputFile);

    allocate2DArrayMemory(unpackedImage, unpackedImage->height, unpackedImage->width);
    unpackedImage = unpackImage(unpackedImage, packedImage, 5);

    freeImageData(packedImage);

    return unpackedImage;
}

int ebcBlockGetImageData(const char *fileName, ImageData *unpackedImage, ImageData *packedBlockImage) {
    FILE *inputFile = fopen(fileName, "rb");
    int check = inputCheck(fileName, unpackedImage, inputFile, MAGIC_NUMBER_EC);
    if (check) {
        reportError(check, fileName);
        return check;
    }
    allocate2DArrayMemory(packedBlockImage, unpackedImage->height, unpackedImage->width);
    packedBlockImage->height = 0;
    packedBlockImage->width = 0;
    fread(&packedBlockImage->data[0][0], 1, 1, inputFile);
    for (int row = 0, end = 0; row < unpackedImage->height && !end; row++) {
        for (int column = 0; column < unpackedImage->width && !end; column++) {
            int check = fread(&packedBlockImage->data[row][column], 1, 1, inputFile);

            if (check != 1) {
                packedBlockImage->height = row;
                packedBlockImage->width = column;
                end = 1;
            }
        }
    }
    fclose(inputFile);
    return SUCCESS;
}

int ebcRandomGetImageData(const char *fileName, ImageData *unpackedImageData, ImageData *packedRandomImage,
                           PixelBlock **pixelBlocks, int packSize, int magicNumber) {
    FILE *inputFile = fopen(fileName, "rb");
    int check = inputCheck(fileName, unpackedImageData, inputFile, magicNumber);
    if (check) {
        reportError(check, fileName);
        return check;
    }

    int numberOf3x3Block = numberOf3x3Blocks(unpackedImageData->height, unpackedImageData->width);
    int numberOfParadigmBlocks = 32;
    if (numberOf3x3Block < 32) {
        numberOfParadigmBlocks = numberOf3x3Block;
    }

    ImageData *packedParadigmImage = malloc(sizeof(ImageData));
    allocate2DArrayMemory(packedParadigmImage, unpackedImageData->height, 9);
    packedParadigmImage->height = numberOfParadigmBlocks;
    packedParadigmImage->width = 9;
    ImageData *unpackedParadigmImage = malloc(sizeof(ImageData));
    allocate2DArrayMemory(unpackedParadigmImage, numberOfParadigmBlocks, 9);
    fread(&packedParadigmImage->data[0][0], 1, 1, inputFile);

    unsigned int buffer;
    int bit_in_buffer = 0;
    unpackedParadigmImage->height = 0;
    unpackedParadigmImage->width = 0;
    for (int row = 0, end = 0; row < packedParadigmImage->height && !end; row++) {
        for (int column = 0; column < packedParadigmImage->width && !end; column++) {
            if (unpackedParadigmImage->height == numberOfParadigmBlocks && unpackedParadigmImage->width == 0) {
                end = 1;
                break;
            }
            fread(&packedParadigmImage->data[row][column], 1, 1, inputFile);
            for (int bit = 0; bit < 8; bit++) {
                if (unpackedParadigmImage->height == numberOfParadigmBlocks && unpackedParadigmImage->width == 0) {
                    end = 1;
                    break;
                }
                buffer |= getBit(packedParadigmImage->data[row][column], 8 - bit - 1) << (8 - bit_in_buffer - 1);
                bit_in_buffer++;
                if (bit_in_buffer == packSize) {
                    unpackedParadigmImage->data[unpackedParadigmImage->height][unpackedParadigmImage->width] = buffer >> (8 -
                                                                                                                          packSize);
                    unpackedParadigmImage->width++;
                    if (unpackedParadigmImage->width == 9) {
                        unpackedParadigmImage->height++;
                        unpackedParadigmImage->width = 0;
                    }
                    bit_in_buffer = 0;
                    buffer = 0;
                }
            }
        }
    }

    *pixelBlocks = (PixelBlock *)malloc(numberOfParadigmBlocks * sizeof(PixelBlock));
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        for (int row = 0; row < 3; row++) {
            for (int column = 0; column < 3; column++) {
                (*pixelBlocks)[i].pixels[row][column] = unpackedParadigmImage->data[i][row * 3 + column];
            }
        }
    }

    packedRandomImage->height = unpackedImageData->height;
    packedRandomImage->width = unpackedImageData->width;
    allocate2DArrayMemory(packedRandomImage, unpackedImageData->height, unpackedImageData->width);
    fread(&packedRandomImage->data[0][0], 1, 1, inputFile);
    for (int row = 0, end = 0; row < unpackedImageData->height && !end; row++) {
        for (int column = 0; column < unpackedImageData->width && !end; column++) {
            int check = fread(&packedRandomImage->data[row][column], 1, 1, inputFile);
            if (check != 1) {
                packedRandomImage->height = row;
                packedRandomImage->width = column;
                end = 1;
                break;
            }
        }
    }

    fclose(inputFile);
    return SUCCESS;
}