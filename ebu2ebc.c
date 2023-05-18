#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "inputData.h"
#include "outputFile.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2

int main(int argc, char **argv) {

    // validate that user has enter 2 arguments (plus the executable name)
    // check arg count

    if (argc == 1) {
        printf("Usage: ebu2ebc file1 file2");
        return SUCCESS;
    }
    if (argc != 3) { 
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }

    Data *data = malloc(sizeof(Data));

    data->fileName = argv[1];
    data->format[0] = 'e';
    data->format[1] = 'u';
    
    int check = inputData_EU2EC(data);
    if (check) {
        freeData(data);
        free(data);
        return check;
    }

    check = outputFile_EC(data,argv[2]);
    if (!check) {
        printf("CONVERTED\n");
    }
    
    freeData(data);
    free(data);
    return check;

}