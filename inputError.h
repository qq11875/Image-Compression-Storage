
void freeData(Data *data);

int allocate2DArrayMemory(ImageData *imageData, int height, int width);

int isDataTooMuch(Data *data, FILE *inputFile);

int inputData_EU(Data *data, char *fileName);

int inputData_EC(Data *data, char *fileName);

int inputDataR(Data *data, char *fileName);