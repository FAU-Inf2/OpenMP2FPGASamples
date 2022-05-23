#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
int kernel_cache_misses = 0;
int kernel_cache_hits = 0;
int data_cache_misses = 0;
int data_cache_hits = 0;
// #define WITH_CACHE_COUNT

int linearize(int x,int y,int bndx,int bndy)
{
  
#pragma HLS INLINE REGION
  if (x >= bndx || y >= bndy) 
    return - 1;
  return bndy * x + y;
}

void apply_filter(double *kernel,double *data,double *result,const int N,const int M)
{
  
#pragma HLS INLINE REGION
  for (int x = 0; x < N; x++) {
    
#pragma UNEXPRESSED
    for (int y = 0; y < N; y++) {
      
#pragma HLS UNROLL factor=3
      double weightsum = (double )0;
      double sum = (double )0;
      for (int i = 0; i < M; i++) {
        
#pragma UNEXPRESSED
        for (int j = 0; j < M; j++) {
          
#pragma UNEXPRESSED
          double weight = kernel[linearize(i,j,M,M)];
          int target = linearize(x + i,y + j,N,N);
          if (target < 0) 
            continue; 
          weightsum += weight;
          sum += weight * data[target];
        }
      }
      result[linearize(x,y,N,N)] = sum / weightsum;
    }
  }
}

void OutlinedFunction0(int OrkaParam0,int OrkaParam1,double *OrkaParam2,double *OrkaParam3,double *OrkaParam4)
{
  
#pragma HLS INLINE REGION
;
  int param_pack_temp_variable0 = (int )OrkaParam0;
  int param_pack_temp_variable1 = (int )OrkaParam1;
  double *param_pack_temp_variable2 = (double *)OrkaParam2;
  double *param_pack_temp_variable3 = (double *)OrkaParam3;
  double *param_pack_temp_variable4 = (double *)OrkaParam4;
{
{
      apply_filter(param_pack_temp_variable2,param_pack_temp_variable3,param_pack_temp_variable4,param_pack_temp_variable0,param_pack_temp_variable1);
    }
  }
}

