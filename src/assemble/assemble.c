#include <stdlib.h>
#include <stdio.h>
#include "filewriter.h"
#include "filereader.h"

int main(int argc, char **argv) {
  
  //validate all arguments have been given
  if(argc != 3) {
    fprintf( stderr, "Error: invalid arguments!, \n" );
    exit(1);
  }
  //initialise input and output files
  initFileReader(argv[1]);
  initFileWriter(argv[2]);

  readAssFile();

  rclose();
  wclose();
  return EXIT_SUCCESS;
}
