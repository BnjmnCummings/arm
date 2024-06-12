#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  
  //validate all arguments have been given
  if(argc != 3) {
    fprintf( stderr, "Error: invalid arguments!, \n" );
    exit(1);
  }

  //get file pointer from first argument
  FILE *in = fopen(argv[1], "r");
  if(in == NULL) {
    fprintf( stderr, "Error: can’t open %s\n", argv[1] );
    exit(1);
  }

  //create output file and pass into filewriter.c
  FILE *out = fopen(argv[2], "w");
  if(in == NULL) {
    fprintf( stderr,"Error: can’t open %s\n", argv[2] );
    exit(1);
  }
  
  //pass file pointer into filereader.c

  return EXIT_SUCCESS;
}
