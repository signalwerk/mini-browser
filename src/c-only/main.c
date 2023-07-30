#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");

    if (!file)
    {
        printf("Error opening file: %s\n", filename);
        return 1;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate a buffer to hold the file contents
    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer)
    {
        printf("Memory allocation error.\n");
        fclose(file);
        return 1;
    }

    // Read the file contents into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate the buffer

    fclose(file);

    // Initialize the parser
    struct Parser parser;
    Parser_init(&parser, buffer);

    // Parse the stylesheet
    // struct stylesheet *parsedStylesheet = Parser_parseStylesheet(&parser);
    Parser_printTokens(&parser);

    // TODO: Perform any necessary operations on the parsedStylesheet struct
    // For example, you can iterate through the entities and print their contents.

    // Cleanup: free the buffer and the parsedStylesheet
    free(buffer);
    // Parser_cleanupStylesheet(parsedStylesheet);

    return 0;
}
