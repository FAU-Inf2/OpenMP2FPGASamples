#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define LENGTH 10000

void mov_dat(int* from, int* to){
	for(int i = 0; i < LENGTH ; i++){
		to[i] = from[i];
	}
}

void sort(int* array){
	char change = 1;
	int buf1[LENGTH];
	int buf2[LENGTH];
	mov_dat(array,buf1);
	for(int i = 0; i < LENGTH; i++){
		for(int j = i%2; j < LENGTH-1; j+=2){
			if(buf1[j]>buf1[j+1]){
				buf2[j] = buf1[j+1];
				buf2[j+1] = buf1[j];
			} else {
				buf2[j] = buf1[j];
				buf2[j+1] = buf1[j+1];
			}
		}
		mov_dat(buf2,buf1);
	}
	mov_dat(buf1,array);
}

int main(int argc, char **argv) {
  srand(0);
  int length = 10000;
  int* array = malloc(sizeof(int)*LENGTH);
  for(int i = 0; i < LENGTH; i++){
	  array[i] = rand();
  }

  #pragma omp target map(tofrom : array[0:LENGTH])
  {
	sort(array);
  }
  for(int i = 0; i < LENGTH; i++){
	  printf("%d\n", array[i]);
  }
  free(array);
}
