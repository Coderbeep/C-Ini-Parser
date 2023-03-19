#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "readfile.h"


#define ALLOC_LINES 512
#define ALLOC_CHARS 512


char ** read_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    char c;

    size_t lineNumber = 0;
    size_t charNumber = 0;
    char **lines = malloc(sizeof(char*) * 1024);

    while (true) {
        c = fgetc(file);

        // if the end of file is reached, break the loop
        if (feof(file)) {
            // if the end of file is reached, and number of chars is not zero
            // realloc the line in lines, end the string and increment the number of lines
            if (charNumber != 0){
                lines[lineNumber] = realloc(lines[lineNumber], charNumber + 1);
                lines[lineNumber][charNumber] = '\0';

                lineNumber++;
            }
            break;
        }
        // when starting new line, allocate some memory for it
        if (charNumber == 0)
            lines[lineNumber] = malloc(ALLOC_CHARS);

        lines[lineNumber][charNumber] = c; // add a char to proper place in the array
        charNumber++; // increment the number of chars in the line

        // handle the end of line sign
        if (c == '\n') {
            lines[lineNumber] = realloc(lines[lineNumber], charNumber + 1);
            lines[lineNumber][charNumber] = '\0'; // end of string

            lineNumber++;
            charNumber = 0;

            // check if there is enough space for another line in the array
            // there is need for adding more space if the lineNumber is a multiple of the ALLOC_LINES
            // here, as a default it is set to 512, so initially the array of lines has 512 places
            if (lineNumber % ALLOC_LINES == 0) {
                lines = realloc(lines, sizeof(char *) * (lineNumber + ALLOC_LINES));
            }
        }
        // otherwise, check if there is space for more chars in the line
        else if (charNumber % ALLOC_CHARS == 0) {
            lines[lineNumber] = realloc(lines[lineNumber], charNumber + ALLOC_CHARS);
        }
    }
    // reallocate the memory of lines to match the exact size of this array
    // that's why there is 'char *' and not 'char'
    // it means that we allocate memory of each line as an array of chars (string)
    lines = realloc(lines, sizeof(char *) * (lineNumber + 1));
    lines[lineNumber] = NULL;
    
    // for (int i = 0; lines[i] != NULL; i++) {
    //     int len = strlen(lines[i]);
    //     lines[i][len] = '\0';
    // }

    fclose(file);
    return lines;
}
