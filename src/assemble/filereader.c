// FILE *in = fopen( "input.txt", "r" );
//   if( in == NULL ) {
//      fprintf( stderr, "Can't read input.txt\n" );
//      return 1;
//   }
//   char buffer[10];
//   int lineno = 1;
//   bool linestart = true;
//   while( fgets(buffer, sizeof(buffer), in) != NULL ) {
//     if( linestart ) { printf( "%4d ", lineno++ ); }
//     fputs( buffer, stdout );
//     int len = strlen(buffer);
//     linestart = buffer[len-1] == '\n';
//   }
//   fclose(in);
//   return 0;