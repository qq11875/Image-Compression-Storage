
ImageData *getImageDataFromEC(const char *fileName, int *errorCode);

int ebcBlockGetImageData(const char *fileName, ImageData *unpackedImage, ImageData *packedBlockImage);

int ebcRandomGetImageData(const char *fileName, ImageData *unpackedImageData, ImageData *packedRandomImage,
                          PixelBlock **pixelBlocks, int packSize, int magicNumber);