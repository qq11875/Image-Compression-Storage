#include <stdlib.h>

#include "data.h"

#define SUCCESS 0
#define BAD_MALLOC 5

void freeImageData(ImageData *imageData) {
    if (imageData->data != NULL) {
        free(imageData->data);
    }
    if (imageData->dataBlock != NULL) {
        free(imageData->dataBlock);
    }
    free(imageData);
}

int allocate2DArrayMemory(ImageData *imageData, int height, int width) {
    // allocate memory for the 2D array
    imageData->data = (unsigned int **)malloc(height * sizeof(unsigned int *));
    imageData->dataBlock = (unsigned int *)malloc(height * width * sizeof(unsigned int));

    // validate that the memory has been allocated correctly
    if (!imageData->data || !imageData->dataBlock) {
        return BAD_MALLOC;
    }

    // set the pointers in the array to point to the correct location in the block
    for (int row = 0; row < height; row++) {
        imageData->data[row] = imageData->dataBlock + row * width;
    }

    return SUCCESS;
}