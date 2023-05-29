#include "filereader.h"

FILE *open_file(char* filename) {
    return fopen(filename, "r");
}

char* read_line(FILE *file) {
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, file) == -1) {
      free(line);      
      return NULL;
    }
    else
      return line;
}

void close_file(FILE *file) {
    fclose(file);
}
