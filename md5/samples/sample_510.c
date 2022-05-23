#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/*
 * Simple MD5 implementation
 *
 * Source: https://gist.github.com/creationix/4710780
 */
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
#define MAX_LEN 1024

void md5(uint8_t *initial_msg,size_t initial_len,uint32_t *h0,uint32_t *h1,uint32_t *h2,uint32_t *h3)
{
  
#pragma HLS INLINE REGION
  uint32_t r[] = {((uint32_t )7), ((uint32_t )12), ((uint32_t )17), ((uint32_t )22), ((uint32_t )7), ((uint32_t )12), ((uint32_t )17), ((uint32_t )22), ((uint32_t )7), ((uint32_t )12), ((uint32_t )17), ((uint32_t )22), ((uint32_t )7), ((uint32_t )12), ((uint32_t )17), ((uint32_t )22), ((uint32_t )5), ((uint32_t )9), ((uint32_t )14), ((uint32_t )20), ((uint32_t )5), ((uint32_t )9), ((uint32_t )14), ((uint32_t )20), ((uint32_t )5), ((uint32_t )9), ((uint32_t )14), ((uint32_t )20), ((uint32_t )5), ((uint32_t )9), ((uint32_t )14), ((uint32_t )20), ((uint32_t )4), ((uint32_t )11), ((uint32_t )16), ((uint32_t )23), ((uint32_t )4), ((uint32_t )11), ((uint32_t )16), ((uint32_t )23), ((uint32_t )4), ((uint32_t )11), ((uint32_t )16), ((uint32_t )23), ((uint32_t )4), ((uint32_t )11), ((uint32_t )16), ((uint32_t )23), ((uint32_t )6), ((uint32_t )10), ((uint32_t )15), ((uint32_t )21), ((uint32_t )6), ((uint32_t )10), ((uint32_t )15), ((uint32_t )21), ((uint32_t )6), ((uint32_t )10), ((uint32_t )15), ((uint32_t )21), ((uint32_t )6), ((uint32_t )10), ((uint32_t )15), ((uint32_t )21)};
  uint32_t k[] = {(0xd76aa478), (0xe8c7b756), ((uint32_t )0x242070db), (0xc1bdceee), (0xf57c0faf), ((uint32_t )0x4787c62a), (0xa8304613), (0xfd469501), ((uint32_t )0x698098d8), (0x8b44f7af), (0xffff5bb1), (0x895cd7be), ((uint32_t )0x6b901122), (0xfd987193), (0xa679438e), ((uint32_t )0x49b40821), (0xf61e2562), (0xc040b340), ((uint32_t )0x265e5a51), (0xe9b6c7aa), (0xd62f105d), ((uint32_t )0x02441453), (0xd8a1e681), (0xe7d3fbc8), ((uint32_t )0x21e1cde6), (0xc33707d6), (0xf4d50d87), ((uint32_t )0x455a14ed), (0xa9e3e905), (0xfcefa3f8), ((uint32_t )0x676f02d9), (0x8d2a4c8a), (0xfffa3942), (0x8771f681), ((uint32_t )0x6d9d6122), (0xfde5380c), (0xa4beea44), ((uint32_t )0x4bdecfa9), (0xf6bb4b60), (0xbebfbc70), ((uint32_t )0x289b7ec6), (0xeaa127fa), (0xd4ef3085), ((uint32_t )0x04881d05), (0xd9d4d039), (0xe6db99e5), ((uint32_t )0x1fa27cf8), (0xc4ac5665), (0xf4292244), ((uint32_t )0x432aff97), (0xab9423a7), (0xfc93a039), ((uint32_t )0x655b59c3), (0x8f0ccc92), (0xffeff47d), (0x85845dd1), ((uint32_t )0x6fa87e4f), (0xfe2ce6e0), (0xa3014314), ((uint32_t )0x4e0811a1), (0xf7537e82), (0xbd3af235), ((uint32_t )0x2ad7d2bb), (0xeb86d391)};
   *h0 = ((uint32_t )0x67452301);
   *h1 = 0xefcdab89;
   *h2 = 0x98badcfe;
   *h3 = ((uint32_t )0x10325476);
  int new_len = (int )(((initial_len + ((unsigned long )8)) / ((unsigned long )64) + ((unsigned long )1)) * ((unsigned long )64) - ((unsigned long )8));
  uint8_t msg[1144];
  for (size_t i = (size_t )0; i < ((unsigned long )(new_len + 64)); i++) {
    
#pragma HLS PIPELINE II=30 
    msg[i] = ((uint8_t )0);
  }
  for (size_t i = (size_t )0; i < initial_len; i++) {
    
#pragma HLS UNROLL factor=3
    msg[i] = initial_msg[i];
  }
  msg[initial_len] = ((uint8_t )128);
  uint32_t bits_len = (uint32_t )(((unsigned long )8) * initial_len);
  for (size_t i = (size_t )0; i < ((unsigned long )4); i++) {
    
#pragma UNEXPRESSED
    msg[((unsigned long )new_len) + i] = ((uint8_t )(((int )((uint8_t )bits_len)) << i * ((unsigned long )8)));
  }
  int offset;
  for (offset = 0; offset < new_len; offset += 512 / 8) {
    
#pragma HLS PIPELINE II=3 
    uint32_t *w = (uint32_t *)(msg + offset);
    uint32_t a =  *h0;
    uint32_t b =  *h1;
    uint32_t c =  *h2;
    uint32_t d =  *h3;
    uint32_t i;
    for (i = ((uint32_t )0); i < ((unsigned int )64); i++) {
      
#pragma HLS UNROLL factor=2
      uint32_t f;
      uint32_t g;
      if (i < ((unsigned int )16)) {
        f = b & c | ~b & d;
        g = i;
      }
       else if (i < ((unsigned int )32)) {
        f = d & b | ~d & c;
        g = (((unsigned int )5) * i + ((unsigned int )1)) % ((unsigned int )16);
      }
       else if (i < ((unsigned int )48)) {
        f = b ^ c ^ d;
        g = (((unsigned int )3) * i + ((unsigned int )5)) % ((unsigned int )16);
      }
       else {
        f = c ^ (b | ~d);
        g = ((unsigned int )7) * i % ((unsigned int )16);
      }
      uint32_t temp = d;
      d = c;
      c = b;
      b = b + (a + f + k[i] + w[g] << r[i] | a + f + k[i] + w[g] >> ((unsigned int )32) - r[i]);
      a = temp;
    }
     *h0 += a;
     *h1 += b;
     *h2 += c;
     *h3 += d;
  }
}

void OutlinedFunction0(char *OrkaParam0,size_t OrkaParam1,uint32_t (*OrkaParam2)[1020])
{
  
#pragma HLS INLINE OFF
;
  char *param_pack_temp_variable0 = (char *)OrkaParam0;
  size_t param_pack_temp_variable1 = (size_t )OrkaParam1;
  uint32_t (*param_pack_temp_variable2)[1020] = (uint32_t (*)[1020])OrkaParam2;
{
{
      for (uint8_t off = (uint8_t )0; ((int )off) < 0xFF; off++) {
        
#pragma UNEXPRESSED
        uint8_t input[1024];
        for (size_t i = (size_t )0; i < param_pack_temp_variable1; i++) {
          
#pragma UNEXPRESSED
          input[i] = ((uint8_t )(((int )param_pack_temp_variable0[i]) + ((int )off)));
        }
        uint32_t h0;
        uint32_t h1;
        uint32_t h2;
        uint32_t h3;
        md5(input,param_pack_temp_variable1,&h0,&h1,&h2,&h3);
        ( *param_pack_temp_variable2)[((size_t )off) * ((unsigned long )4) + ((unsigned long )0)] = h0;
        ( *param_pack_temp_variable2)[((size_t )off) * ((unsigned long )4) + ((unsigned long )1)] = h1;
        ( *param_pack_temp_variable2)[((size_t )off) * ((unsigned long )4) + ((unsigned long )2)] = h2;
        ( *param_pack_temp_variable2)[((size_t )off) * ((unsigned long )4) + ((unsigned long )3)] = h3;
      }
    }
  }
}

