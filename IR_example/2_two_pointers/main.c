#include<stdio.h>
#include<stdlib.h>


int foo (int * a , int* b) {
  return *a + *b;
}

int goo (int * ptr, char * ptr2) {
  return *ptr + *ptr2;
}


int main(int argc, char * argv[]) {

 int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

 int d = foo(a, a + 4);

 int e[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
 int f = goo(e + 4, (char *) e);

 return d + f;

}
