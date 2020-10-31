/**
 * APPROXIMATE PATTERN MATCHING
 *
 * INF560 X2016
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <mpi.h>

#define APM_DEBUG 0
char * 
read_input_file( char * filename, int * size )
{
    char * buf ;
    off_t fsize;
    int fd = 0 ;
    int n_bytes = 1 ;
    /* Open the text file */
    fd = open( filename, O_RDONLY ) ;
    if ( fd == -1 ) {
        fprintf( stderr, "Unable to open the text file <%s>\n", filename ) ;
        return NULL ;
    }

    /* Get the number of characters in the textfile */
    fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    /* TODO check return of lseek */

#if APM_DEBUG
    printf( "File length: %lld\n", fsize ) ;
#endif

    /* Allocate data to copy the target text */
    buf = (char *)malloc( fsize * sizeof ( char ) ) ;
    if ( buf == NULL ) {
        fprintf( stderr, "Unable to allocate %lld byte(s) for main array\n", fsize ) ;
        return NULL ;
    }

    n_bytes = read( fd, buf, fsize ) ;
    if ( n_bytes != fsize ) {
        fprintf( stderr, 
                "Unable to copy %lld byte(s) from text file (%d byte(s) copied)\n",
                fsize, n_bytes) ;
        return NULL ;
    }

#if APM_DEBUG
    printf( "Number of read bytes: %d\n", n_bytes ) ;
#endif

    *size = n_bytes ;
    close( fd ) ;
    return buf ;
}

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int levenshtein(char *s1, char *s2, int len, int * column) {
    unsigned int x, y, lastdiag, olddiag;
    for (y = 1; y <= len; y++) {
        column[y] = y;
    }
    for (x = 1; x <= len; x++) {
        column[0] = x;
        lastdiag = x-1 ;
        for (y = 1; y <= len; y++) {
            olddiag = column[y];
            column[y] = MIN3(
              column[y] + 1, 
              column[y-1] + 1, 
              lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1)
              );
            lastdiag = olddiag;
        }
    }
    return(column[len]);
}

int 
main( int argc, char ** argv )
{
  char ** pattern ;
  char * filename ;
  int approx_factor = 0 ;
  int nb_patterns = 0 ;
  int i, j ;
  char * buf ;
  struct timeval t1, t2;
  double duration ;
  int n_bytes ;
  int * n_matches ;
  int comm_rank;
  int comm_size;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
  
  /* Check number of arguments */
  if ( argc < 4 ) {
    printf( "Usage: %s approximation_factor dna_database pattern1 pattern2 ...\n", argv[0] ) ;
    return 1 ;
  }

  /* Get the distance factor */
  approx_factor = atoi( argv[1] ) ;
  
  /* Grab the filename containing the target text */
  filename = argv[2] ;
	
  /* Get the number of patterns that the user wants to search for */
  nb_patterns = argc - 3 ;
  
  /* Fill the pattern array */
  pattern = (char **)malloc( nb_patterns * sizeof( char * ) ) ;
  if ( pattern == NULL ) {
    fprintf( stderr, "Unable to allocate array of pattern of size %d\n", nb_patterns ) ;
    return 1 ;
  }
  
  /* Grab the patterns */
  for ( i = 0 ; i < nb_patterns ; i++ ) {
    int l ;
    l = strlen(argv[i+3]) ;
    if ( l <= 0 ) {
        fprintf( stderr, "Error while parsing argument %d\n", i+3 ) ;
        return 1 ;
    }
    pattern[i] = (char *)malloc( (l+1) * sizeof( char ) ) ;
    if ( pattern[i] == NULL ) {
        fprintf( stderr, "Unable to allocate string of size %d\n", l ) ;
        return 1 ;
    }
    strncpy( pattern[i], argv[i+3], (l+1) ) ;
  }

  if (comm_rank == 0) {
    printf("Approximate Pattern Mathing: looking for %d pattern(s) in file %s w/ distance of %d with %d processes\n", nb_patterns, filename, approx_factor, comm_size ) ;
  }
  
  buf = read_input_file( filename, &n_bytes) ;
  if ( buf == NULL ) {
    return 1 ;
  }
  
  /* Allocate the array of matches */
  n_matches = (int *)malloc( nb_patterns * sizeof( int ) ) ;
  if ( n_matches == NULL ) {
    fprintf( stderr, "Error: unable to allocate memory for %ldB\n", nb_patterns * sizeof( int ) ) ;
    return 1 ;
  }

  /*****
   * BEGIN MAIN LOOP
   ******/
  
  MPI_Barrier(MPI_COMM_WORLD);
  /* Timer start */
  gettimeofday(&t1, NULL);

  for ( i = 0 ; i < nb_patterns ; i++ ) {
    int size_pattern = strlen(pattern[i]) ;
    
    int * column ;
    n_matches[i] = 0;
    
    column = (int *)malloc( (size_pattern+1) * sizeof( int ) );
    if ( column == NULL ) {
      fprintf( stderr, "Error: unable to allocate memory for column (%ldB)\n", (size_pattern+1) * sizeof( int ) ) ;
      return 1 ;
    }

    int chunk_size=n_bytes/comm_size;    
    int lower=chunk_size*comm_rank;
    int upper=chunk_size*(comm_rank+1);
    
    if (comm_rank == comm_size-1)
      upper = n_bytes;
    for (j =  lower; j < upper ; j++ ) {
      int distance = 0 ;
      int size ;
#if APM_DEBUG
  if ( j % 100 == 0 ) {
    printf( "Procesing byte %d (out of %d)\n", j, n_bytes ) ;
  }
#endif
      size = size_pattern ;
      if ( n_bytes - j < size_pattern ) {
          size = n_bytes - j ;
      }
          
	    distance = levenshtein( pattern[i], &buf[j], size, column ) ;
      if (distance <= approx_factor) {
        n_matches[i]++;
      }
    }

    if (comm_rank != 0 ){
      MPI_Send(&n_matches[i], nb_patterns, MPI_INT, 0, comm_rank, MPI_COMM_WORLD);
    } else {
      int N;
      int rank = 0;
      for (rank = 1; rank < comm_size; rank++) {
        MPI_Recv(&N, nb_patterns, MPI_INT, rank, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        n_matches[i] += N;
      }
    }
    free( column );
  }

  /* Timer stop */
  gettimeofday(&t2, NULL);
  MPI_Barrier(MPI_COMM_WORLD);

  if (comm_rank == 0) {
    duration = (t2.tv_sec -t1.tv_sec)+((t2.tv_usec-t1.tv_usec)/1e6);
    printf( "APM done in %lf s\n", duration ) ;
  }
  
  /*****
   * END MAIN LOOP
   ******/
  if(comm_rank==0) {
    for ( i = 0 ; i < nb_patterns ; i++ ) {
      printf( "Number of matches for pattern <%s>: %d\n", pattern[i], n_matches[i] ) ;
    }
  }
  MPI_Finalize();
  return 0 ;
}
