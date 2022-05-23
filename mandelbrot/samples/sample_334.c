#include "mandelbrot.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "camera.h"
#include "cmdline.h"
static double camera_center_x;
static double camera_center_y;
static double camera_width;
static uint32_t resolution_width;
static uint32_t resolution_height;
static double zoom_factor = 1.6;
static struct cmdline data = {.res_x = (uint32_t )500, .res_y = (uint32_t )500, .iterations = (uint32_t )100};

void OutlinedFunction0(size_t OrkaParam0,size_t OrkaParam1,int *OrkaParam2,double *OrkaParam3)
{
  
#pragma HLS INLINE OFF
;
  size_t param_pack_temp_variable0 = (size_t )OrkaParam0;
  size_t param_pack_temp_variable1 = (size_t )OrkaParam1;
  int *param_pack_temp_variable2 = (int *)OrkaParam2;
  double *param_pack_temp_variable3 = (double *)OrkaParam3;
{
{
      for (size_t i = (size_t )0; i < param_pack_temp_variable0; i++) {
        
#pragma HLS UNROLL factor=2
        param_pack_temp_variable2[i] = - 1;
        double result_real = (double )0;
        double result_imag = (double )0;
        double c_real = param_pack_temp_variable3[i];
        double c_imag = param_pack_temp_variable3[i + param_pack_temp_variable0];
        for (size_t iter = (size_t )0; iter < param_pack_temp_variable1; iter++) {
          
#pragma HLS PIPELINE II=23 
          double new_real = result_real * result_real - result_imag * result_imag + c_real;
          double new_imag = result_real * result_imag * ((double )2) + c_imag;
          double abs = new_real * new_real + new_imag * new_imag;
          if (abs > ((double )4)) {
            param_pack_temp_variable2[i] = ((int )iter);
            break; 
          }
          result_real = new_real;
          result_imag = new_imag;
        }
      }
    }
  }
}

