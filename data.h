
// data structure of the data
typedef struct pixelBlock {
    unsigned int pixels[3][3];
} PixelBlock;

typedef struct imageData {
    unsigned int **data;
    unsigned int *dataBlock;
    int width;
    int height;
} ImageData;