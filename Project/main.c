#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
 * Returns the array of strings from splitting the string by the separators 
 * 
 * Parameters:
 *      char [] - an array of chars to perform the splitting on
 * 
 * Returns:
 *      char ** - null-terminated array of strings
 */

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


struct keyValuePair {
    char *key;
    char *value;
};


struct section {
    char *name;
    size_t length;
    struct keyValuePair *keysValues;
    int numKeysValues; // used to iterate through keyValuePairs in the structure
};


char ** string_split(char str[], char separators[]) {
    char **strings = malloc(16 * sizeof(char*));
    char *word = strtok(str, separators);
    int numberOfWords = 0;

    while (word != NULL) {
        strings[numberOfWords] = strdup(word);
        numberOfWords++;
        word = strtok(NULL, separators);
    }

    strings = realloc(strings, numberOfWords * sizeof(char *));
    strings[numberOfWords] = NULL;

    return strings;
}

void print_array(char *str[]) {
    for (int i = 0; str[i] != NULL; i++) 
        printf("%s", str[i]);
}

char * add_brackets(char str[]) {
    int len = strlen(str);
    char* new_str = malloc((len + 3) * sizeof(char));

    sprintf(new_str, "[%s]", str);
    return new_str;
}

char *get_value(char **lines, char *sectionName, char *key) {
    int indexBegin = 0;
    int indexEnd = 0;
    int flag = 0;
    char *value = malloc(strlen(sectionName) + strlen(key) + 32);
    // define the borders of the section
    for (int i = 0; lines[i] != NULL; i++) {
        if (strstr(lines[i], sectionName) != NULL) {
            indexBegin = i;
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        sprintf(value, "Failed to find section %s.", sectionName);
        value = realloc(value, strlen(value));
        return value;
    }
    flag = 0;

    for (int i = indexBegin; lines[i] != NULL; i++) {
        indexEnd = i;
        if (strlen(lines[i]) == 1) {
            break;
        }
    }

    for (int i = indexBegin; i <= indexEnd; i++) {
        char *lineToSplit = strdup(lines[i]);
        char **splittedLine = string_split(lineToSplit, " ");
        if (strcmp(splittedLine[0], key) == 0) {
            flag = 1;
            value = strdup(splittedLine[2]);
        }
        free(lineToSplit);
        for (int i = 0; splittedLine[i] != NULL; i++) 
            free(splittedLine[i]);
        free(splittedLine);
        if (flag == 1)
            break;
    }

    if (flag == 0) {
        sprintf(value, "Failed to find key \"%s\" in section %s.", key, sectionName);
        value = realloc(value, strlen(value));
        return value;
    }

    return value;
}

struct section* parse_ini_file(char **lines) {
    struct section *sections = NULL;

    int numberOfSections = 0;  
    int lineLength;
    char *line, *sectionName;
    int sectionID = -1;
    int numKeyValues = 0;

    for (int i = 0; lines[i] != NULL; i++) {
        line = lines[i];
        lineLength = strlen(line);
        printf("%s", line);
        if (line[0] == '[' && line[lineLength - 2] == ']') {
            sectionID++;
            numberOfSections++;
            
            sections = realloc(sections, sizeof(struct section) * numberOfSections);

            sectionName = line;
            memmove(sectionName, sectionName + 1, strlen(sectionName));
            sectionName[strlen(sectionName) - 2] = '\0';
            sections[sectionID].name = sectionName;
            sections[sectionID].length = strlen(sections[sectionID].name);
            sections[sectionID].keysValues = NULL;
            // printf("%s\n", sectionName);
            numKeyValues = 0;
            continue;
        }

        numKeyValues++;
        
        if (strlen(line) != 1) {
            char *lineToSplit = strdup(line);
            char **splittedLine = string_split(lineToSplit, " ");

            char *key = malloc(sizeof(char) * (strlen(splittedLine[0]) + 1));
            char *value = malloc(sizeof(char) * (strlen(splittedLine[2]) + 1));
            // printf("%s\n", lineToSplit);
            strcpy(key, splittedLine[0]);
            strcpy(value, splittedLine[2]);
            value[strcspn(value, "\n")] = '\0';

            sections[sectionID].keysValues = realloc(sections[sectionID].keysValues, sizeof(struct keyValuePair) * numKeyValues);

            struct keyValuePair *pair = &sections[sectionID].keysValues[numKeyValues - 1];
            pair->key = key;
            pair->value = value;
            sections[sectionID].numKeysValues = numKeyValues;
            
            free(lineToSplit);
            for (int i = 0; splittedLine[i] != NULL; i++) 
                free(splittedLine[i]);
            free(key);
            free(value);
        }
    }
    // for (int j = 0; sections[2].keysValues[j].key != NULL; j++){
    //     printf("%s  %s", sections[2].keysValues[j].key, sections[2].keysValues[j].value);
    // }
    return sections;
}


int main(int argc, char **argv) {
    char ** lines = read_file("example-4.5.ini");
    // char ** parameters = string_split(argv[2], ".");
    // char *value = get_value(lines, add_brackets(parameters[0]), parameters[1]);
    // printf("%s", value);
    struct section* sections = parse_ini_file(lines);
    // for (int i = 0; sections[i].name != NULL; i++){
    //     printf("Section: [%s]\n", sections[i].name);
    //     for (int j = 0; j < sections[i].numKeysValues; j++) {
    //         printf("%s %s\n", sections[i].keysValues[j].key, sections[i].keysValues[j].value);
    //     }
    // }

    printf("X");

    // freeing the contents of the file
    for (int i = 0; lines[i] != NULL; i++) 
        free(lines[i]);
    free(lines);

    // freeing the parameters given by user
    // for (int i = 0; parameters[i] != NULL; i++) 
    //     free(parameters[i]);
    // free(parameters);

    // free(value);
    return 0;
}
