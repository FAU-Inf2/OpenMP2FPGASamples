#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

struct sha256_ctx_s 
{
  uint8_t data[64];
  uint32_t datalen;
  uint32_t bitlen[2];
  uint32_t state[8];
}
;
typedef struct sha256_ctx_s SHA256_CTX;
uint32_t k[64] = {((uint32_t )0x428a2f98), ((uint32_t )0x71374491), (0xb5c0fbcf), (0xe9b5dba5), ((uint32_t )0x3956c25b), ((uint32_t )0x59f111f1), (0x923f82a4), (0xab1c5ed5), (0xd807aa98), ((uint32_t )0x12835b01), ((uint32_t )0x243185be), ((uint32_t )0x550c7dc3), ((uint32_t )0x72be5d74), (0x80deb1fe), (0x9bdc06a7), (0xc19bf174), (0xe49b69c1), (0xefbe4786), ((uint32_t )0x0fc19dc6), ((uint32_t )0x240ca1cc), ((uint32_t )0x2de92c6f), ((uint32_t )0x4a7484aa), ((uint32_t )0x5cb0a9dc), ((uint32_t )0x76f988da), (0x983e5152), (0xa831c66d), (0xb00327c8), (0xbf597fc7), (0xc6e00bf3), (0xd5a79147), ((uint32_t )0x06ca6351), ((uint32_t )0x14292967), ((uint32_t )0x27b70a85), ((uint32_t )0x2e1b2138), ((uint32_t )0x4d2c6dfc), ((uint32_t )0x53380d13), ((uint32_t )0x650a7354), ((uint32_t )0x766a0abb), (0x81c2c92e), (0x92722c85), (0xa2bfe8a1), (0xa81a664b), (0xc24b8b70), (0xc76c51a3), (0xd192e819), (0xd6990624), (0xf40e3585), ((uint32_t )0x106aa070), ((uint32_t )0x19a4c116), ((uint32_t )0x1e376c08), ((uint32_t )0x2748774c), ((uint32_t )0x34b0bcb5), ((uint32_t )0x391c0cb3), ((uint32_t )0x4ed8aa4a), ((uint32_t )0x5b9cca4f), ((uint32_t )0x682e6ff3), ((uint32_t )0x748f82ee), ((uint32_t )0x78a5636f), (0x84c87814), (0x8cc70208), (0x90befffa), (0xa4506ceb), (0xbef9a3f7), (0xc67178f2)};

uint32_t ROTRIGHT(uint32_t a,uint32_t b)
{
  
#pragma HLS INLINE REGION
  return a >> b | a << ((unsigned int )32) - b;
}

uint32_t CH(uint32_t x,uint32_t y,uint32_t z)
{
  
#pragma HLS INLINE OFF
  return x & y ^ ~x & z;
}

uint32_t MAJ(uint32_t x,uint32_t y,uint32_t z)
{
  
#pragma HLS INLINE REGION
  return x & y ^ x & z ^ y & z;
}

uint32_t EP0(uint32_t x)
{
  
#pragma HLS INLINE OFF
  return ROTRIGHT(x,(uint32_t )2) ^ ROTRIGHT(x,(uint32_t )13) ^ ROTRIGHT(x,(uint32_t )22);
}

uint32_t SIG0(uint32_t x)
{
  
#pragma HLS INLINE REGION
  return ROTRIGHT(x,(uint32_t )7) ^ ROTRIGHT(x,(uint32_t )18) ^ x >> 3;
}

uint32_t SIG1(uint32_t x)
{
  
#pragma HLS INLINE REGION
  return ROTRIGHT(x,(uint32_t )17) ^ ROTRIGHT(x,(uint32_t )19) ^ x >> 10;
}

uint32_t EP1(uint32_t x)
{
  
#pragma HLS INLINE OFF
  return ROTRIGHT(x,(uint32_t )6) ^ ROTRIGHT(x,(uint32_t )11) ^ ROTRIGHT(x,(uint32_t )25);
}

void sha256_transform(SHA256_CTX *ctx,uint8_t data[])
{
  
#pragma HLS INLINE REGION
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
  uint32_t e;
  uint32_t f;
  uint32_t g;
  uint32_t h;
  uint32_t i;
  uint32_t j;
  uint32_t t1;
  uint32_t t2;
  uint32_t m[64];
  for ((i = ((uint32_t )0) , j = ((uint32_t )0)); i < ((unsigned int )16); (++i , j += ((unsigned int )4))) {
    
#pragma HLS UNROLL factor=3
    m[i] = ((uint32_t )(((int )data[j]) << 24 | ((int )data[j + ((unsigned int )1)]) << 16 | ((int )data[j + ((unsigned int )2)]) << 8 | ((int )data[j + ((unsigned int )3)])));
  }
  for (; i < ((unsigned int )64); ++i) {
    
#pragma HLS PIPELINE II=13
    m[i] = SIG1(m[i - ((unsigned int )2)]) + m[i - ((unsigned int )7)] + SIG0(m[i - ((unsigned int )15)]) + m[i - ((unsigned int )16)];
  }
  a = ctx -> state[0];
  b = ctx -> state[1];
  c = ctx -> state[2];
  d = ctx -> state[3];
  e = ctx -> state[4];
  f = ctx -> state[5];
  g = ctx -> state[6];
  h = ctx -> state[7];
  for (i = ((uint32_t )0); i < ((unsigned int )64); ++i) {
    
#pragma HLS UNROLL factor=1
    t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
    t2 = EP0(a) + MAJ(a,b,c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }
  ctx -> state[0] += a;
  ctx -> state[1] += b;
  ctx -> state[2] += c;
  ctx -> state[3] += d;
  ctx -> state[4] += e;
  ctx -> state[5] += f;
  ctx -> state[6] += g;
  ctx -> state[7] += h;
}

void my_memset(uint8_t *data,uint8_t val,uint32_t len)
{
  
#pragma HLS INLINE OFF
  for (int i = 0; ((unsigned int )i) < len; i++) {
    
#pragma UNEXPRESSED
    data[i] = val;
  }
}

void sha256_final(SHA256_CTX *ctx,uint8_t hash[])
{
  
#pragma HLS INLINE OFF
  uint32_t i;
  i = ctx -> datalen;
  if (ctx -> datalen < ((unsigned int )56)) {
    ctx -> data[i++] = ((uint8_t )0x80);
    while(i < ((unsigned int )56))
      ctx -> data[i++] = ((uint8_t )0x00);
  }
   else {
    ctx -> data[i++] = ((uint8_t )0x80);
    while(i < ((unsigned int )64))
      ctx -> data[i++] = ((uint8_t )0x00);
    sha256_transform(ctx,ctx -> data);
    my_memset(ctx -> data,(uint8_t )0,(uint32_t )56);
  }
  if (ctx -> bitlen[0] > 0xffffffff - ctx -> datalen * ((unsigned int )8)) 
    ++ctx -> bitlen[1];
  ctx -> bitlen[0] += ctx -> datalen * ((unsigned int )8);
  ;
  ctx -> data[63] = ((uint8_t )ctx -> bitlen[0]);
  ctx -> data[62] = ((uint8_t )(ctx -> bitlen[0] >> 8));
  ctx -> data[61] = ((uint8_t )(ctx -> bitlen[0] >> 16));
  ctx -> data[60] = ((uint8_t )(ctx -> bitlen[0] >> 24));
  ctx -> data[59] = ((uint8_t )ctx -> bitlen[1]);
  ctx -> data[58] = ((uint8_t )(ctx -> bitlen[1] >> 8));
  ctx -> data[57] = ((uint8_t )(ctx -> bitlen[1] >> 16));
  ctx -> data[56] = ((uint8_t )(ctx -> bitlen[1] >> 24));
  sha256_transform(ctx,ctx -> data);
  for (i = ((uint32_t )0); i < ((unsigned int )4); ++i) {
    
#pragma UNEXPRESSED
    hash[i] = ((uint8_t )(ctx -> state[0] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )4)] = ((uint8_t )(ctx -> state[1] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )8)] = ((uint8_t )(ctx -> state[2] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )12)] = ((uint8_t )(ctx -> state[3] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )16)] = ((uint8_t )(ctx -> state[4] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )20)] = ((uint8_t )(ctx -> state[5] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )24)] = ((uint8_t )(ctx -> state[6] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
    hash[i + ((unsigned int )28)] = ((uint8_t )(ctx -> state[7] >> ((unsigned int )24) - i * ((unsigned int )8) & ((unsigned int )0x000000ff)));
  }
}

void sha256_init(SHA256_CTX *ctx)
{
  
#pragma HLS INLINE REGION
  ctx -> datalen = ((uint32_t )0);
  ctx -> bitlen[0] = ((uint32_t )0);
  ctx -> bitlen[1] = ((uint32_t )0);
  ctx -> state[0] = ((uint32_t )0x6a09e667);
  ctx -> state[1] = 0xbb67ae85;
  ctx -> state[2] = ((uint32_t )0x3c6ef372);
  ctx -> state[3] = 0xa54ff53a;
  ctx -> state[4] = ((uint32_t )0x510e527f);
  ctx -> state[5] = 0x9b05688c;
  ctx -> state[6] = ((uint32_t )0x1f83d9ab);
  ctx -> state[7] = ((uint32_t )0x5be0cd19);
}

void sha256_update(SHA256_CTX *ctx,uint8_t data[],uint32_t len)
{
  
#pragma HLS INLINE REGION
  uint32_t i;
  for (i = ((uint32_t )0); i < len; ++i) {
    
#pragma UNEXPRESSED
    ctx -> data[ctx -> datalen] = data[i];
    ctx -> datalen++;
    if (ctx -> datalen == ((unsigned int )64)) {
      sha256_transform(ctx,ctx -> data);
      if (ctx -> bitlen[0] > 0xffffffff - ((unsigned int )512)) 
        ++ctx -> bitlen[1];
      ctx -> bitlen[0] += ((unsigned int )512);
      ;
      ctx -> datalen = ((uint32_t )0);
    }
  }
}
// Top Function

void sha256(uint8_t data[64],uint8_t result[32])
{
  
#pragma HLS INLINE REGION
  SHA256_CTX ctx;
  sha256_init(&ctx);
  sha256_update(&ctx,data,(uint32_t )64);
  sha256_final(&ctx,result);
}

void mov256(uint8_t *in,uint8_t *target)
{
  
#pragma HLS INLINE
  for (int i = 0; i < 32; i++) {
    
#pragma UNEXPRESSED
    target[i] = in[i];
  }
}

void OutlinedFunction0(int OrkaParam0,uint8_t (*OrkaParam1)[64],uint8_t (*OrkaParam2)[32])
{
  
#pragma HLS INLINE
;
  int param_pack_temp_variable0 = (int )OrkaParam0;
  uint8_t (*param_pack_temp_variable1)[64] = (uint8_t (*)[64])OrkaParam1;
  uint8_t (*param_pack_temp_variable2)[32] = (uint8_t (*)[32])OrkaParam2;
{
{
      uint8_t buffer1[64];
      uint8_t buffer2[32];
      for (int i = 0; i < 64; i++) {
        
#pragma UNEXPRESSED
        buffer1[i] = ((uint8_t )0);
      }
      mov256( *param_pack_temp_variable1,buffer1);
      for (int i = 0; i < param_pack_temp_variable0; i++) {
        
#pragma UNEXPRESSED
        sha256(buffer1,buffer2);
        mov256(buffer2,buffer1);
      }
      mov256(buffer1, *param_pack_temp_variable2);
    }
  }
}

