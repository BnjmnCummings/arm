#include "assemble.h"
#include "filewriter.h"
#include "filereader.h"
#include "symtable.h"

int main(int argc, char **argv) {
  
  //validate all arguments have been given
  if(argc != 3) {
    fprintf( stderr, "Error: invalid arguments!, \n" );
    exit(EXIT_FAILURE);
  }

  //initialise input and output files
  initFileReader(argv[1]);
  initFileWriter(argv[2]);
  init_table();

  //first pass
  read_file(&read_symbol);
  //second pass
  read_file(&read_line);

  //file closing
  rclose();
  wclose();
  return EXIT_SUCCESS;
}
