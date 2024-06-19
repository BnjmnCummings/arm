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
  init_file_reader(argv[1]);
  init_file_writer(argv[2]);
  init_table();

  //first pass
  read_file(&read_symbol);

  //second pass
  read_file(&read_line);

  //file closing
  r_close();
  w_close();

  //free heap allocated resources
  free_table();

  return EXIT_SUCCESS;
}
