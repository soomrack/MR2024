#include <stdio.h>
#include <stddef.h>
#include "matrix.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

  srand(time(NULL) + getpid());

  Matrix *a = malloc(sizeof(Matrix)), *b = malloc(sizeof(Matrix)), *c = malloc(sizeof(Matrix));
  create_random_matrix(a, 1, 1, -2, 2, 3);
  //create_random_matrix(b, 1, 1, -2, 2, 3);
  //malloc_matrix(1, 1, c);
  
  print_matrix(*a, 3);
  //print_matrix(*b, 3);

  printf("%d\n", expMatrix(*a, c, 6));
    
  print_matrix(*c, 6);
  return 0;
}
