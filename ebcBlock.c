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

#define PACK_SIZE 5

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Usage: ebcBlock file1 file2");
        return SUCCESS;
    }
    if (argc != 3) {
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }

    int errorCode = SUCCESS;
    ImageData *unpackedImage = getImageDataFromEC(argv[1], &errorCode);
    if (errorCode != SUCCESS) {
        return errorCode;
    }

    ImageData *unpackedBlockImage = ebcBlock_Pack(unpackedImage);

    ImageData *packedBlockImage = packData(unpackedBlockImage, unpackedImage->height, unpackedImage->width, PACK_SIZE);

    freeImageData(unpackedBlockImage);

    outputImageToFile(argv[2], unpackedImage, packedBlockImage, "EC");

    freeImageData(unpackedImage);

    freeImageData(packedBlockImage);


    printf("CONVERTED\n");
    return SUCCESS;
}