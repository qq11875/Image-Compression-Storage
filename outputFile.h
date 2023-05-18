
void outputImageToFile(char const *fileName, const ImageData *unpackedImage, const ImageData *packedBlockImage,
                       char *magicNumber);

void
outputToFile(char *const fileName, const ImageData *unpackedImage, const ImageData *packedImage, char *magicNumber);

void
outputParadigmBlocksAndImageToFile(const char *fileName, const ImageData *unpackedImageData, int numberOfParadigmBlocks,
                                   const ImageData *packedR32Image, const ImageData *packedParadigmImage,
                                   char *magicNumber);