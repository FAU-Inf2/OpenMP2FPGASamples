#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#define LENGTH 10000

void mov_dat(int *from,int *to)
{
  
#pragma HLS INLINE OFF
  for (int i = 0; i < 10000; i++) {
    
#pragma HLS UNROLL factor=957
    to[i] = from[i];
  }
}

void sort(int *array)
{
  
#pragma HLS INLINE OFF
  char change = (char )1;
  int buf1[10000];
  int buf2[10000];
  mov_dat(array,buf1);
  for (int i = 0; i < 10000; i++) {
    
#pragma HLS UNROLL factor=3
    for (int j = i % 2; j < 10000 - 1; j += 2) {
      
#pragma HLS PIPELINE II=5  rewind
      if (buf1[j] > buf1[j + 1]) {
        buf2[j] = buf1[j + 1];
        buf2[j + 1] = buf1[j];
      }
       else {
        buf2[j] = buf1[j];
        buf2[j + 1] = buf1[j + 1];
      }
    }
    mov_dat(buf2,buf1);
  }
  mov_dat(buf1,array);
}

void OutlinedFunction0(int *OrkaParam0)
{
  
#pragma HLS INLINE OFF
;
  int *param_pack_temp_variable0 = (int *)OrkaParam0;
{
{
      sort(param_pack_temp_variable0);
    }
  }
}

