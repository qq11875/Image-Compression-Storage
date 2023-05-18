#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "inputData.h"
#include "memoryUtils.h"
#include "outputFile.h"
#include "pack-unpack.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MALLOC 5
#define MAGIC_NUMBER_E7 0x3745

#define PACK_SIZE 5

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: ebcR32 file1 file2");
        return SUCCESS;
    }
    if (argc != 3) {
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }

    ImageData *unpackedImageData = malloc(sizeof(ImageData));

    ImageData *packedR32Image = malloc(sizeof(ImageData));

    PixelBlock *pixelBlocks = NULL;

    ebcRandomGetImageData(argv[1], unpackedImageData, packedR32Image, &pixelBlocks, PACK_SIZE, MAGIC_NUMBER_E7);

    ImageData *unpackedR32Image = unpackImage(unpackedImageData, packedR32Image, PACK_SIZE);

    freeImageData(packedR32Image);

    ImageData *outputImage = replaceIndexWithParadigmBlock(unpackedImageData, unpackedR32Image, pixelBlocks);

    free(pixelBlocks);
    freeImageData(unpackedR32Image);

    outputImage->height = unpackedImageData->height;
    outputImage->width = unpackedImageData->width;

    ImageData *packedImage = packData(outputImage, outputImage->height, outputImage->width, 5);

    outputToFile(argv[2], unpackedImageData, packedImage, "ec");
    freeImageData(unpackedImageData);
    freeImageData(outputImage);
    freeImageData(packedImage);

    printf("CONVERTED\n");

    return 0;
}