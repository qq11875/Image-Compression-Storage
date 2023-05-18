#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "memoryUtils.h"
#include "pack-unpack.h"
#include "inputData.h"
#include "outputFile.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MALLOC 5

#define PACK_SIZE 5

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: ebcR32 file1 file2 seed");
        return SUCCESS;
    }
    if (argc != 4) {
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }

    srand(atoi(argv[3]));

    int errorCode = SUCCESS;
    ImageData *unpackedImageData = getImageDataFromEC(argv[1], &errorCode);
    if (errorCode != SUCCESS) {
        return errorCode;
    }

    int numberOf3x3Block = numberOf3x3Blocks(unpackedImageData->height, unpackedImageData->width);
    int numberOfParadigmBlocks = 32;
    if (numberOf3x3Block < 32) {
        numberOfParadigmBlocks = numberOf3x3Block;
    }

    PixelBlock *pixelBlocks = getParadigmBlocks(unpackedImageData, numberOfParadigmBlocks);

    ImageData *unpackedR32Image = replaceImageWithParadigmBlockIndex(unpackedImageData, numberOfParadigmBlocks, pixelBlocks);

    ImageData *packedR32Image = packData(unpackedR32Image, unpackedImageData->height, unpackedImageData->width, PACK_SIZE);

    freeImageData(unpackedR32Image);

    ImageData *unpackedParadigmImage = getParadigmBlockImage(numberOfParadigmBlocks, pixelBlocks);
    free(pixelBlocks);

    ImageData *packedParadigmImage = packData(unpackedParadigmImage, numberOfParadigmBlocks, 9, PACK_SIZE);

    freeImageData(unpackedParadigmImage);

    outputParadigmBlocksAndImageToFile(argv[2], unpackedImageData, numberOfParadigmBlocks, packedR32Image, packedParadigmImage, "E5");

    freeImageData(unpackedImageData);
    freeImageData(packedR32Image);
    freeImageData(packedParadigmImage);

    printf("CONVERTED\n");
    return SUCCESS;
}