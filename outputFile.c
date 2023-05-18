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

void outputImageToFile(char const *fileName, const ImageData *unpackedImage, const ImageData *packedBlockImage,
                       char *magicNumber) {
    FILE *outputFile = fopen(fileName, "wb");
    fprintf(outputFile, "%s\n%d %d\n", magicNumber, unpackedImage->height, unpackedImage->width);
    for (int row = 0, end = 0; row < unpackedImage->height && !end; row++) {
        for (int column = 0; column < unpackedImage->width && !end; column++) {
            if (row == packedBlockImage->height && column == packedBlockImage->width) {
                end = 1;
                break;
            }
            fwrite(&packedBlockImage->data[row][column], 1, 1, outputFile);
        }
    }

    fclose(outputFile);
}

void
outputToFile(char *const fileName, const ImageData *unpackedImage, const ImageData *packedImage, char *magicNumber) {
    FILE *outputFile = fopen(fileName, "wb");
    // write the header data in one block
    fprintf(outputFile, "%s\n%d %d\n", magicNumber, unpackedImage->height, unpackedImage->width);

    // iterate though the array and print out pixel values
    for (int row = 0, end = 0; row < unpackedImage->height && !end; row++) {
        for (int column = 0; column < unpackedImage->width && !end; column++) {
            if (row == packedImage->height && column == packedImage->width) {
                end = 1;
                break;
            }
            fwrite(&packedImage->data[row][column], 1, 1, outputFile);
        }
    }

    fclose(outputFile);
}

void
outputParadigmBlocksAndImageToFile(const char *fileName, const ImageData *unpackedImageData, int numberOfParadigmBlocks,
                                   const ImageData *packedR32Image, const ImageData *packedParadigmImage,
                                   char *magicNumber) {
    FILE *outputFile = fopen(fileName, "wb");
    fprintf(outputFile, "%s\n%d %d\n", magicNumber, unpackedImageData->height, unpackedImageData->width);

    for (int i = 0, end = 0; i < numberOfParadigmBlocks && !end; i++) {
        for (int j = 0; j < 9 && !end; j++) {
            if (i == packedParadigmImage->height && j == packedParadigmImage->width) {
                end = 1;
                break;
            }
            fwrite(&packedParadigmImage->data[i][j], 1, 1, outputFile);
        }
    }
    fprintf(outputFile, "\n");

    for (int row = 0, end = 0; row < unpackedImageData->height && !end; row++) {
        for (int column = 0; column < unpackedImageData->width && !end; column++) {
            if (row == packedR32Image->height && column == packedR32Image->width) {
                end = 1;
                break;
            }
            fwrite(&packedR32Image->data[row][column], 1, 1, outputFile);
        }
    }

    fclose(outputFile);
}