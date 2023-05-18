#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    FILE *inputFile = fopen(argv[1], "rb");

    char magic[2];
    magic[0] = getc(inputFile);
    magic[1] = getc(inputFile);

    int height;
    int width;

    fscanf(inputFile, "%d %d", &height, &width);


    unsigned int *array = malloc(height*width*sizeof(unsigned int));

    FILE *outputFile = fopen(argv[2], "w");

    fprintf(outputFile, "eb\n");
    fprintf(outputFile, "%d %d\n", height, width);

    fread(&magic, 1, 1, inputFile);
    for (int i = 0; i < height*width; i++) {
        fread(&array[i], 1, 1, inputFile);

        if (array[i] < 10) {
            fprintf(outputFile, "  %u", array[i]);
        } else {
            fprintf(outputFile, " %u", array[i]);
        }
        if ((i+1) % width == 0) {
            fprintf(outputFile, "%c", '\n');
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}