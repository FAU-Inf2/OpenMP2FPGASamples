#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#define LENGTH 10000

void mov_dat(int *from,int *to)
{
  
#pragma UNEXPRESSED
  for (int i = 0; i < 10000; i++) {
    
#pragma HLS PIPELINE II=19  rewind
    to[i] = from[i];
  }
}

void sort(int *array)
{
  
#pragma UNEXPRESSED
  char change = (char )1;
  int buf1[10000];
  int buf2[10000];
  mov_dat(array,buf1);
  for (int i = 0; i < 10000; i++) {
    
#pragma UNEXPRESSED
    for (int j = i % 2; j < 10000 - 1; j += 2) {
      
#pragma HLS UNROLL factor=1
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
  
#pragma UNEXPRESSED
;
  int *param_pack_temp_variable0 = (int *)OrkaParam0;
{
{
      sort(param_pack_temp_variable0);
    }
  }
}

