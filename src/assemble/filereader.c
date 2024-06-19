#include "filereader.h"
#include "parser.h"


static FILE *in;

#define INC(addr) (addr += 4)

static bool isLabel(char *line) {
    // TODO(check edge cases, not following rules)
    if (line[strcspn(line, " ") - 1] == ':') {
        line[strcspn(line, " ")] = '\0';
        return true;
    } else {
        return false;
    }
}

void initFileReader(char * filename) {
    in = fopen(filename, "r");
    if(in == NULL) {
        fprintf( stderr, "Error: can't open %s\n", filename );
        exit(EXIT_FAILURE);
    }
}

void read_symbol(char *buffer, int *addr) {
    if (buffer[0] != '\0') {
        if (isLabel(buffer)) {
            buffer[strlen(buffer) - 1] = '\0';
            store_symbol(buffer, *addr);
        } else {
            INC(*addr);
        }
    }
}

void read_line(char *buffer, int *addr) {
    if (!isLabel(buffer) && buffer[0] != '\0') {
        parse_line(buffer, *addr);
        INC(*addr);
    }
}

void read_file(void (*read_func)(char *, int *)) {
    char *buffer = malloc(MAX_LINE_LENGTH);
    int addr = 0;

    while( fgets(buffer, MAX_LINE_LENGTH, in) != NULL ) {
        buffer[strcspn(buffer, "\n")] = '\0';
        (*read_func)(buffer, &addr);
    }

    free(buffer);
}

void rclose() {
    fclose(in);
}
