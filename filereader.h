#ifndef FILEREADER_H
#define FILEREADER_H

#include <stdio.h>

/**
 * Opens a file in read mode
 * @param filename path to the file to open
 * @return pointer to the file or NULL in case of error (e.g., file not found)
 */
FILE *open_file(char* filename);

/**
 * Reads a single line from a file
 * @param file pointer to an open file
 * @return a string of text with the complete line. The memory of the text is automatically allocated and must be freed
 * by the user when this is not needed anymore. If there are no more lines to read the function returns NULL
 */
char* read_line(FILE *file);

/**
 * Closes an open file
 * @param file pointer to the file to be closed
 */
void close_file(FILE *file);

#endif
