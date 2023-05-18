#include <stdlib.h>

#include "data.h"
#include "memoryUtils.h"

#define PACK_SIZE 5
#define ONE_BYTE 8

unsigned int getBit(unsigned int pixel, int bit) {
    return (pixel >> bit) & 0b00000001;
}

int numberOf3x3Blocks(int height, int width) {
    int numberOfBlocks = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            if (row + 2 < height && column + 2 < width) {
                numberOfBlocks++;
            }
        }
    }
    return numberOfBlocks;
}

int getClosestParadigmBlockIndex(PixelBlock *paradigmBlocks, unsigned int **image, int row, int column, int numberOfParadigmBlocks, int heightLimit, int widthLimit) {
    int index = 0;

    int minSumOfDifferencesAndMaxCountOfSamePixel[2];

    // find the most similar image block
    int sumOfDifferencesPerPixel;
    int countOfSamePixel;
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        sumOfDifferencesPerPixel = 0;
        countOfSamePixel = 0;
        for (int j = 0; j < 3 && (row + j) < heightLimit; j++) {
            for (int k = 0; k < 3 && (column + k) < widthLimit; k++) {
                sumOfDifferencesPerPixel += abs((int)paradigmBlocks[i].pixels[j][k] - (int)image[row + j][column + k]);
                if (paradigmBlocks[i].pixels[j][k] == image[row + j][column + k]) {
                    countOfSamePixel++;
                }
            }
        }
        if (i == 0) {
            minSumOfDifferencesAndMaxCountOfSamePixel[0] = sumOfDifferencesPerPixel;
            minSumOfDifferencesAndMaxCountOfSamePixel[1] = countOfSamePixel;
        }
        if (sumOfDifferencesPerPixel == minSumOfDifferencesAndMaxCountOfSamePixel[0]) {
            if (countOfSamePixel > minSumOfDifferencesAndMaxCountOfSamePixel[1]) {
                index = i;
            }
        }
        if (sumOfDifferencesPerPixel < minSumOfDifferencesAndMaxCountOfSamePixel[0]) {
            minSumOfDifferencesAndMaxCountOfSamePixel[0] = sumOfDifferencesPerPixel;
            index = i;
        }
    }

    return index;
}

ImageData *ebcBlock_Pack(const ImageData *unpackedImage) {
    ImageData *unpackedBlockImage = malloc(sizeof(ImageData));
    allocate2DArrayMemory(unpackedBlockImage, unpackedImage->height, unpackedImage->width);
    unpackedBlockImage->height = 0;
    unpackedBlockImage->width = 0;

    int sum = 0;
    int count = 0;
    for (int i = 0; i < unpackedImage->height; i += 3) {
        for (int j = 0; j < unpackedImage->width; j += 3) {
            for (int row = i; row < i + 3 && row < unpackedImage->height; row++) {
                for (int column = j; column < j + 3 && column < unpackedImage->width; column++) {
                    sum += unpackedImage->data[row][column];
                    count++;
                }
            }
            unpackedBlockImage->data[unpackedBlockImage->height][unpackedBlockImage->width] = (double)sum / count + 0.5;
            unpackedBlockImage->width++;
            if (unpackedBlockImage->width == unpackedImage->width) {
                unpackedBlockImage->width = 0;
                unpackedBlockImage->height++;
            }
            // reset the sum and count
            sum = 0;
            count = 0;
        }
    }
    return unpackedBlockImage;
}

void ebcBlock_Unpack(ImageData *unpackedImage, const ImageData *unpackedBlockImage) {
    allocate2DArrayMemory(unpackedImage, unpackedImage->height, unpackedImage->width);
    int tmpHeight = 0;
    int tmpWidth = 0;
    for (int i = 0; i < unpackedImage->height; i += 3) {
        for (int j = 0; j < unpackedImage->width; j += 3) {
            for (int row = i; row < i + 3 && row < unpackedImage->height; row++) {
                for (int column = j; column < j + 3 && column < unpackedImage->width; column++) {
                    unpackedImage->data[row][column] = unpackedBlockImage->data[tmpHeight][tmpWidth];
                }
            }
            tmpWidth++;
            if (tmpWidth == unpackedImage->width) {
                tmpWidth = 0;
                tmpHeight++;
            }
        }
    }
}

PixelBlock *getParadigmBlocks(const ImageData *unpackedImage, int numberOfParadigmBlocks) {
    PixelBlock *pixelBlocks = malloc(numberOfParadigmBlocks * sizeof(PixelBlock));
    int **chosenPos = malloc(numberOfParadigmBlocks * sizeof(int *));
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        chosenPos[i] = malloc(2 * sizeof(int));
    }
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        while (1) {
            int randomRow = rand() % unpackedImage->height;
            int randomColumn = rand() % unpackedImage->width;
            int broke = 0;
            for (int j = 0; j < i; j++) {
                if (chosenPos[j][0] == randomRow && chosenPos[j][1] == randomColumn) {
                    broke = 1;
                    break;
                }
            }
            if (randomRow + 2 < unpackedImage->height && randomColumn + 2 < unpackedImage->width && !broke) {
                chosenPos[i][0] = randomRow;
                chosenPos[i][1] = randomColumn;
                for (int row = 0; row < 3; row++) {
                    for (int column = 0; column < 3; column++) {
                        pixelBlocks[i].pixels[row][column] = unpackedImage->data[randomRow + row][randomColumn + column];
                    }
                }
                break;
            }
        }
    }
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        free(chosenPos[i]);
    }
    free(chosenPos);
    return pixelBlocks;
}

ImageData *replaceImageWithParadigmBlockIndex(ImageData *unpackedImage, int numberOfParadigmBlocks, PixelBlock *pixelBlocks) {
    ImageData *unpackedR32Image = malloc(sizeof(ImageData));
    unpackedR32Image->height = 0;
    unpackedR32Image->width = 0;
    allocate2DArrayMemory(unpackedR32Image, unpackedImage->height, unpackedImage->width);
    for (int i = 0; i < unpackedImage->height; i += 3) {
        for (int j = 0; j < unpackedImage->width; j += 3) {
            int blockIndex = getClosestParadigmBlockIndex(pixelBlocks, unpackedImage->data, i, j, numberOfParadigmBlocks, unpackedImage->height, unpackedImage->width);
            unpackedR32Image->data[unpackedR32Image->height][unpackedR32Image->width] = blockIndex;
            unpackedR32Image->width++;
            if (unpackedR32Image->width == unpackedImage->width) {
                unpackedR32Image->width = 0;
                unpackedR32Image->height++;
            }
        }
    }
    return unpackedR32Image;
}

ImageData *getParadigmBlockImage(int numberOfParadigmBlocks, const PixelBlock *pixelBlocks) {
    ImageData *unpackedParadigmImage = malloc(sizeof(ImageData));
    unpackedParadigmImage->height = 0;
    unpackedParadigmImage->width = 0;
    allocate2DArrayMemory(unpackedParadigmImage, numberOfParadigmBlocks, 9);
    for (int i = 0; i < numberOfParadigmBlocks; i++) {
        for (int row = 0; row < 3; row++) {
            for (int column = 0; column < 3; column++) {
                unpackedParadigmImage->data[unpackedParadigmImage->height][unpackedParadigmImage->width] = pixelBlocks[i].pixels[row][column];
                
                unpackedParadigmImage->width++;
            }
        }
        unpackedParadigmImage->height++;
        unpackedParadigmImage->width = 0;
    }
    return unpackedParadigmImage;
}

ImageData *replaceIndexWithParadigmBlock(ImageData *unpackedImageData, ImageData *unpackedRandomImage, PixelBlock *pixelBlocks) {
    ImageData *outputImage = malloc(sizeof(ImageData));
    outputImage->height = 0;
    outputImage->width = 0;
    allocate2DArrayMemory(outputImage, unpackedImageData->height, unpackedImageData->width);
    for (int i = 0, end = 0; i < unpackedImageData->height && !end; i++) {
        for (int j = 0; j < unpackedImageData->width && !end; j++) {
            if (i == unpackedRandomImage->height && j == unpackedRandomImage->width) {
                end = 1;
                break;
            }
            int index = unpackedRandomImage->data[i][j];
            for (int row = 0; row < 3; row++) {
                for (int column = 0; column < 3; column++) {
                    if (outputImage->height + row < unpackedImageData->height && outputImage->width + column < unpackedImageData->width) {
                        outputImage->data[outputImage->height + row][outputImage->width + column] = pixelBlocks[index].pixels[row][column];
                    }
                }
            }
            outputImage->width += 3;
            if (outputImage->width >= unpackedImageData->width) {
                outputImage->height += 3;
                outputImage->width = 0;
            }
        }
    }
    return outputImage;
}

ImageData *packData(const ImageData *unpackedData, int heightLimit, int widthLimit, int packSize) {
    ImageData *packedR32Image = malloc(sizeof(ImageData));
    packedR32Image->height = 0;
    packedR32Image->width = 0;
    allocate2DArrayMemory(packedR32Image, heightLimit, widthLimit);
    unsigned int buffer = 0;
    int bit_int_buffer = 0;
    for (int row = 0, end = 0; row < heightLimit && !end; row++) {
        for (int column = 0; column < widthLimit && !end; column++) {
            if (row == unpackedData->height && column == unpackedData->width) {
                end = 1;
                break;
            }
            for (int bit = 0; bit < packSize; bit++) {
                buffer |= getBit(unpackedData->data[row][column], packSize - bit - 1) << (ONE_BYTE - bit_int_buffer - 1);
                bit_int_buffer++;
                if (bit_int_buffer == ONE_BYTE) {
                    packedR32Image->data[packedR32Image->height][packedR32Image->width] = buffer;
                    packedR32Image->width++;
                    if (packedR32Image->width == widthLimit) {
                        packedR32Image->width = 0;
                        packedR32Image->height++;
                    }
                    buffer = 0;
                    bit_int_buffer = 0;
                }
            }
        }
    }
    if (bit_int_buffer > 0) {
        packedR32Image->data[packedR32Image->height][packedR32Image->width] = buffer;
        packedR32Image->width++;
        if (packedR32Image->width == widthLimit) {
            packedR32Image->width = 0;
            packedR32Image->height++;
        }
    }
    return packedR32Image;
}

ImageData *unpackImage(const ImageData *unpackedImage, const ImageData *packedImage, int packSize) {
    ImageData *unpackedBlockImage = malloc(sizeof(ImageData));
    allocate2DArrayMemory(unpackedBlockImage, unpackedImage->height, unpackedImage->width);
    unpackedBlockImage->height = 0;
    unpackedBlockImage->width = 0;
    unsigned int buffer = 0;
    int bit_in_buffer = 0;
    int end = 0;

    for (int row = 0; row < unpackedImage->height && !end; row++) {
        for (int column = 0; column < unpackedImage->width && !end; column++) {
            if (row == packedImage->height && column == packedImage->width) {
                unpackedBlockImage->height = unpackedImage->height;
                unpackedBlockImage->width = unpackedImage->width;
                end = 1;
                break;
            }

            for (int bit = 0; bit < ONE_BYTE; bit++) {
                // place the bit in the buffer
                buffer |= getBit(packedImage->data[row][column], ONE_BYTE - bit - 1) << (ONE_BYTE - bit_in_buffer - 1);
                bit_in_buffer++;

                // if the buffer is full, save it to the unpacked data
                if (bit_in_buffer == packSize && !end) {
                    unpackedBlockImage->data[unpackedBlockImage->height][unpackedBlockImage->width] = buffer >> (ONE_BYTE - packSize);

                    // update the size of the unpacked data
                    unpackedBlockImage->width++;
                    if (unpackedBlockImage->width == unpackedImage->width) {
                        unpackedBlockImage->width = 0;
                        unpackedBlockImage->height++;
                    }

                    // reset the buffer
                    buffer = 0;
                    bit_in_buffer = 0;
                }
            }
        }
    }


    return unpackedBlockImage;
}
