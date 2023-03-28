#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

char ** split(char *sentence, char separators[]) {
    char **splitArray = malloc(8 * sizeof(char *));
    char *ptr = strtok(sentence, separators);
    int numberOfWords = 0;

    // loop ends if there are three words in the array
    // or there are no more than two words in the text to split
    while(ptr != NULL) {
        splitArray[numberOfWords] = strdup(ptr);
        numberOfWords++;
        ptr = strtok(NULL, separators);
        if(numberOfWords == 3)
            break;
    }
    splitArray = realloc(splitArray, sizeof(splitArray) * sizeof(char *));
    return splitArray;
}

struct section *structurize_data(char **lines) {
    int sect_alloc = 16;
    int keyVal_alloc = 16;
    struct section *sections = malloc(sizeof(struct section) * sect_alloc);
    int numberOfSections = 0;
    int numKeyValues = 0;


    for (int i = 0; lines[i] != NULL; i++) {
        if (lines[i][0] == '[' && lines[i][strlen(lines[i]) - 2] == ']') {
            numberOfSections++;

            // allocate more space for data if needed
            if (numberOfSections >= sect_alloc) {
                sect_alloc += 16;
                sections = realloc(sections, sizeof(struct section) * (sect_alloc));
            }

            // prepare the section name
            char *sectionName = lines[i];
            memmove(sectionName, sectionName + 1, strlen(sectionName));
            sectionName[strlen(sectionName) - 2] = '\0';

            sections[numberOfSections - 1].length = (size_t) strlen(sectionName);
            sections[numberOfSections - 1].name = sectionName;
            sections[numberOfSections - 1].keysValues = malloc(sizeof(struct keyValuePair) * (keyVal_alloc));
            int keyVal_alloc = 16;
            numKeyValues = 0;
            continue;
        }

        if (strlen(lines[i]) != 1 && lines[i][0] != ';') {
            numKeyValues++;
            char *lineToSplit = strdup(lines[i]);
            char **splittedArray = split(lineToSplit, " ");

            char *key = malloc(sizeof(char) * (strlen(splittedArray[0]) + 1));
            char *value = malloc(sizeof(char) * (strlen(splittedArray[2]) + 1));
            strcpy(key, splittedArray[0]);
            strcpy(value, splittedArray[2]);
            value[strcspn(value, "\n")] = '\0';
            
            if (numKeyValues >= keyVal_alloc) {
                keyVal_alloc += 16;
                sections[numberOfSections - 1].keysValues = realloc(sections[numberOfSections - 1].keysValues, sizeof(struct keyValuePair) * keyVal_alloc);
            }

            struct keyValuePair pair;
            pair.key = key;
            pair.value = value;
            sections[numberOfSections - 1].keysValues[numKeyValues - 1] = pair;
            sections[numberOfSections - 1].numKeysValues = numKeyValues;

            for (int i = 0; i < 3; i++) 
                free(splittedArray[i]);
            free(splittedArray);
            free(lineToSplit);
        }
        sections[numberOfSections - 1].keysValues = realloc(sections[numberOfSections - 1].keysValues, sizeof(struct keyValuePair) * keyVal_alloc);
    }
    // reallocate the data so it perfectly fits

    sections = realloc(sections, sizeof(struct section) * (numberOfSections + 1));
    sections[numberOfSections].name = NULL; // to indicate the last section

    for (int i = 0; sections[i].name != NULL; i++) { 
        sections[i].keysValues = realloc(sections[i].keysValues, sizeof(struct keyValuePair) * sections[i].numKeysValues);
    }
    return sections;
}

void print_structurized_data(struct section *sections) {
    for (int i = 0; sections[i].name != NULL; i++) {
        for (int j = 0; j < sections[i].numKeysValues; j++) {
            printf("%d %s %s\n", i, sections[i].keysValues[j].key, sections[i].keysValues[j].value);
        }
    }
}

char *get_value(struct section *sections, char *sectionName, char *key) {
    int flag = 0, sectionID, valueID;

    for (int i = 0; sections[i].name != NULL; i++) {
        if (strcmp(sections[i].name, sectionName) == 0) {
            sectionID = i;
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        char *value = malloc(sizeof(char) * (25 + strlen(sectionName) + 1 + 1));
        sprintf(value, "Failed to find section [%s].", sectionName);
        return value;
    }

    flag = 0;



    for (int j = 0; j < sections[sectionID].numKeysValues; j++) {
        if (strcmp(sections[sectionID].keysValues[j].key, key) == 0) {
            valueID = j;
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        char *value = malloc(sizeof(char) * (strlen(sectionName) + strlen(key) + 36 + 1));
        sprintf(value, "Failed to find key \"%s\" in section [%s].", key, sectionName);
        return value;
    }

    return strdup(sections[sectionID].keysValues[valueID].value);
}

bool isNumber(char *string) {
    for (size_t i = 0; i < strlen(string); i++) {
        if (!isdigit(string[i])) {
            return false;
        }
    }
    return true;
}

int checkIfProper(struct section *sections, char *sectionName, char *key) {
    int flag = 0, sectionID;

    for (int i = 0; sections[i].name != NULL; i++) {
        if (strcmp(sections[i].name, sectionName) == 0) {
            sectionID = i;
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        return 0;
    }
    flag = 0;

    for (int j = 0; j < sections[sectionID].numKeysValues; j++) {
        if (strcmp(sections[sectionID].keysValues[j].key, key) == 0) {
            flag = 1;
            break;
        }
    }

    if (flag == 0) {
        return 0;
    }
    return 1;
}

int checkIfAlnum(char *sectionName, char *key) {
    for (int i = 0; i < strlen(sectionName); i++) {
        if (isalnum(sectionName[i]) == 0)
            return 0;
    }

    for (int i = 0; i < strlen(key); i++) {
        if (isalnum(key[i]) == 0)
            return 0;
    }

    return 1;
}

int ifAlnumDashes(char *word){
    for (int i = 0; i < strlen(word); i++) {
        if (isalnum(word[i]) == 0 && word[i] != '-') {
            return 0;
        }
    }
    return 1;
}

void detectInvalidIdentifiers(struct section* sections) {
    for (int i = 0; sections[i].name != NULL; i++) {
        if (ifAlnumDashes(sections[i].name) == 0) {
            printf("Invalid section name [%s] found in file.\n", sections[i].name);
            exit(0);
        }
        for (int j = 0; j < sections[i].numKeysValues; j++) {
            if (ifAlnumDashes(sections[i].keysValues[j].key) == 0) {
                printf("Invalid key [%s] found in file.\n", sections[i].keysValues[j].key);
                exit(0);
            }
        }
    }
}

// TODO create a function that would check if the key or section has invalid name
// invalid names are those containing smothing beyond dashes, letters and numbers

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    char **lines = read_file(filename);
    struct section* sections = structurize_data(lines);
    detectInvalidIdentifiers(sections);
    // // identify the expression command 
    if(strcmp(argv[2], "expression") == 0) {
        char **initExpression = split(argv[3], "\"");
        char **expression = split(initExpression[0], " ");
        char *sign = expression[1];
        char **firstExpression = split(expression[0], ".");
        char **secondExpression = split(expression[2], ".");

        if (!checkIfProper(sections, firstExpression[0], firstExpression[1])) {
            char *value = get_value(sections, firstExpression[0], firstExpression[1]);
            printf("%s\n", value);
        }
        else if (!checkIfProper(sections, secondExpression[0], secondExpression[1])) {
            char *value = get_value(sections, secondExpression[0], secondExpression[1]);
            printf("%s\n", value);
        }
        else {
            char *value1 = get_value(sections, firstExpression[0], firstExpression[1]);
            char *value2 = get_value(sections, secondExpression[0], secondExpression[1]);

            int mode = isNumber(value1) + isNumber(value2);
            switch (mode) {
                case 0: 
                    switch(*sign) {
                        case '+':
                            strcat(value1, value2);
                            puts(value1);
                            break;
                        default:
                            printf("Unsupported operand type.\n");
                            break;
                    }
                    break;

                case 1: 
                    printf("Expressions involving operands of different types are invalid.\n");
                    break;


                case 2: 
                    int a = atoi(value1);
                    int b = atoi(value2);

                    switch(*sign) {
                        case '+':
                            printf("%d\n", a+b);
                            break;
                        case '-':
                            printf("%d\n", a-b);
                            break;
                        case '*':
                            printf("%ld\n", (long) a*b);
                            break;
                        case '/':
                            printf("%f\n", ((float) a / (float) b));
                            break;
                        default:
                            printf("Unsupported operand type.\n");
                            break;
                    }
                    break;
            }
            free(value1);
            free(value2);
        }
        for (int i = 0; i < 2; i++) {
            free(firstExpression[i]);
            free(secondExpression[i]);
        }
        
        free(expression[0]);
        free(expression[1]);
        free(expression[2]);
        free(expression);

        free(firstExpression);
        free(secondExpression);

        free(initExpression[0]);
        free(initExpression);
    }

    // if not expression command then: ./main example.ini section.key
    else {
        char **addressOfValue = split(argv[2], ".");

        char *value = get_value(sections, addressOfValue[0], addressOfValue[1]);
        printf("%s\n", value);

        free(value);
        free(addressOfValue[0]);
        free(addressOfValue[1]);
        free(addressOfValue);
    }

    // memory deallocation
    for (int i = 0; sections[i].name != NULL; i++) {
        for (int j = 0; j < sections[i].numKeysValues; j++) {
            free(sections[i].keysValues[j].key);
            free(sections[i].keysValues[j].value);
        }
        free(sections[i].keysValues);
    }
    free(sections);

    for (int i = 0; lines[i] != NULL; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
