
unsigned int getBit(unsigned int pixel, int bit);

int getClosestParadigmBlockIndex(PixelBlock *paradigmBlocks, unsigned int **image, int row, int column, int numberOfParadigmBlocks, int heightLimit, int widthLimit);

ImageData *replaceImageWithParadigmBlockIndex(ImageData *unpackedImage, int numberOfParadigmBlocks, PixelBlock *pixelBlocks);

ImageData *getParadigmBlockImage(int numberOfParadigmBlocks, const PixelBlock *pixelBlocks);

PixelBlock *getParadigmBlocks(const ImageData *unpackedImage, int numberOfParadigmBlocks);

ImageData *replaceIndexWithParadigmBlock(ImageData *unpackedImageData, ImageData *unpackedRandomImage, PixelBlock *pixelBlocks);

int numberOf3x3Blocks(int height, int width);

ImageData *ebcBlock_Pack(const ImageData *unpackedImage);

void ebcBlock_Unpack(ImageData *unpackedImage, const ImageData *unpackedBlockImage);

ImageData *packData(const ImageData *unpackedData, int heightLimit, int widthLimit, int packSize);

ImageData *unpackImage(const ImageData *unpackedImage, const ImageData *packedImage, int packSize);