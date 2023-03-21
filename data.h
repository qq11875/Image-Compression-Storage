
// data structure of the data

typedef struct data {
    unsigned int **packedData;
    unsigned int *packedDataBlock;
    unsigned int **unpackedData;
    unsigned int *unpackedDataBlock;
    int width;
    int height;
    int packedHeight;
    int packedWidth;
    char format[2];
    char *fileName;
} Data;