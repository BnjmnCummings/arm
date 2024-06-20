#include "filereader.h"
#include "parser.h"

static FILE *in;
static bool comment = false;

#define INC_ADDR(addr) (addr += 4)

//checks if a given string 'line' contains a label
static bool is_label(char *line) {
    if (line[strcspn(line, " ") - 1] == ':') {
        line[strcspn(line, " ")] = '\0';
        return true;
    } else {
        return false;
    }
}

// removes any comments inside the line
static void remove_comments(char *line) {
    // if the line is inside a block comment and there is no close, it is all a comment
    if (comment && !strstr(line, "*/")) {
        strcpy(line, "");
        return;
    }

    char *hold = line;
    char *comstart = NULL;

    while (hold != NULL) {
        if (comment) {
            hold = strstr(line, "*/");
            if (hold != NULL) {
                if (comstart != NULL) {
                    strcpy(comstart, hold + 2);
                    comstart = NULL;
                } else {
                    strcpy(line, hold + 2);
                }
                comment = false;
            }
        } else {
            hold = strstr(line, "/*");
            if (hold != NULL) {
                comstart = hold;
                comment = true;
            }

        }
    }
    if (comstart != NULL) {
        strcpy(comstart, "");
    }
    if (!comment) {
        char *linecom = strstr(line, "//");
        if (linecom != NULL) {
            strcpy(linecom, "");
        }
    }
}

//initialises the input stream as given by cli arguments
void init_file_reader(char * filename) {
    in = fopen(filename, "r");
    if(in == NULL) {
        fprintf( stderr, "Error: can't open %s\n", filename );
        exit(EXIT_FAILURE);
    }
}

//'line function' that handles the "first pass".
//finds labels in line and allocates them memory addresses in the symbol table
void read_symbol(char *buffer, int *addr) {
    if (buffer[0] != '\0') {
        if (is_label(buffer)) {
            buffer[strlen(buffer) - 1] = '\0';
            store_symbol(buffer, *addr);
        } else {
            INC_ADDR(*addr);
        }
    }
}

//'line function' that handles the "second pass".
//passes each line into the parser to be converted into an instruction
void read_line(char *buffer, int *addr) {
    if (!is_label(buffer) && buffer[0] != '\0') {
        parse_line(buffer, *addr);
        INC_ADDR(*addr);
    }
}

//reads a file and handles each line with a given function 'read_func'
void read_file(void (*read_func)(char *, int *)) {
    char *buffer = malloc(MAX_LINE_LENGTH);
    int addr = 0;

    while( fgets(buffer, MAX_LINE_LENGTH, in) != NULL ) {
        buffer[strcspn(buffer, "\n")] = '\0';
        remove_comments(buffer);

        (*read_func)(buffer, &addr);
    }

    free(buffer);
    //rewind to the beginning of the file
    rewind(in);
}

//invokes 'fclose()' on read-file pointer
void r_close(void) {
    fclose(in);
}
