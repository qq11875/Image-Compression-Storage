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
    // validate that user has enter 2 arguments (plus the executable name)
    // check arg count

    if (argc == 1) {
        printf("Usage: ebuUnblock file1 file2");
        return SUCCESS;
    }
    if (argc != 3) {
        return BAD_ARGS;
    }

    ImageData *unpackedImage = malloc(sizeof(ImageData));
    ImageData *packedBlockImage = malloc(sizeof(ImageData));
    int errorCode = ebcBlockGetImageData(argv[1], unpackedImage, packedBlockImage);
    if (errorCode != SUCCESS) {
        return errorCode;
    }

    ImageData *unpackedBlockImage = unpackImage(unpackedImage, packedBlockImage, PACK_SIZE);

    freeImageData(packedBlockImage);

    ebcBlock_Unpack(unpackedImage, unpackedBlockImage);

    freeImageData(unpackedBlockImage);

    ImageData *packedImage = packData(unpackedImage, unpackedImage->height, unpackedImage->width, PACK_SIZE);

    outputToFile(argv[2], unpackedImage, packedImage, "ec");

    freeImageData(unpackedImage);
    freeImageData(packedImage);

    printf("CONVERTED\n");

    return SUCCESS;
}