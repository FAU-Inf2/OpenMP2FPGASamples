#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha3.h"

void mov(char *to,char *from,size_t len)
{
  
#pragma HLS INLINE 
  for (int i = 0; ((unsigned long )i) < len; i++) {
    
#pragma HLS PIPELINE II=10  rewind
    to[i] = from[i];
  }
}
// sha3.c
// 19-Nov-11  Markku-Juhani O. Saarinen <mjos@iki.fi>
// Revised 07-Aug-15 to match with official release of FIPS PUB 202 "SHA3"
// Revised 03-Sep-15 for portability + OpenSSL - style API
// update the state with given number of rounds

void writeSingleByte(uint64_t data[25],size_t pos,uint8_t content)
{
  
#pragma UNEXPRESSED
  uint64_t buf = data[pos / ((unsigned long )8)];
  uint64_t mask = ~(((uint64_t )0xFF) << pos % ((unsigned long )8) * ((unsigned long )8));
  uint64_t content_shifted = ((uint64_t )content) << pos % ((unsigned long )8) * ((unsigned long )8);
  buf &= mask;
  buf |= content_shifted;
  data[pos / ((unsigned long )8)] = buf;
}

uint8_t readSingleByte(uint64_t data[25],size_t pos)
{
  
#pragma UNEXPRESSED
  uint8_t res = (uint8_t )(data[pos / ((unsigned long )8)] >> pos % ((unsigned long )8) * ((unsigned long )8));
  return res;
}

void sha3_keccakf(uint64_t st[25])
{
  
#pragma HLS INLINE 
  const uint64_t keccakf_rndc[24] = {((uint64_t )0x0000000000000001), ((uint64_t )0x0000000000008082), (0x800000000000808a), (0x8000000080008000), ((uint64_t )0x000000000000808b), ((uint64_t )0x0000000080000001), (0x8000000080008081), (0x8000000000008009), ((uint64_t )0x000000000000008a), ((uint64_t )0x0000000000000088), ((uint64_t )0x0000000080008009), ((uint64_t )0x000000008000000a), ((uint64_t )0x000000008000808b), (0x800000000000008b), (0x8000000000008089), (0x8000000000008003), (0x8000000000008002), (0x8000000000000080), ((uint64_t )0x000000000000800a), (0x800000008000000a), (0x8000000080008081), (0x8000000000008080), ((uint64_t )0x0000000080000001), (0x8000000080008008)};
  const int keccakf_rotc[24] = {(1), (3), (6), (10), (15), (21), (28), (36), (45), (55), (2), (14), (27), (41), (56), (8), (25), (43), (62), (18), (39), (61), (20), (44)};
  const int keccakf_piln[24] = {(10), (7), (11), (17), (18), (3), (5), (16), (8), (21), (24), (4), (15), (23), (19), (13), (12), (2), (20), (14), (22), (9), (6), (1)};
  int i;
  int j;
  int r;
  uint64_t t;
  uint64_t bc[5];
  for (r = 0; r < 24; r++) {
    
#pragma HLS PIPELINE II=9 rewind
    for (i = 0; i < 5; i++) {
      
#pragma UNEXPRESSED
      bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
    }
    for (i = 0; i < 5; i++) {
      
#pragma HLS PIPELINE II=23 rewind
      t = bc[(i + 4) % 5] ^ (bc[(i + 1) % 5] << 1 | bc[(i + 1) % 5] >> 64 - 1);
      for (j = 0; j < 25; j += 5) {
        
#pragma UNEXPRESSED
        st[j + i] ^= t;
      }
    }
    t = st[1];
    for (i = 0; i < 24; i++) {
      
#pragma UNEXPRESSED
      j = keccakf_piln[i];
      bc[0] = st[j];
      st[j] = t << keccakf_rotc[i] | t >> 64 - keccakf_rotc[i];
      t = bc[0];
    }
    for (j = 0; j < 25; j += 5) {
      
#pragma HLS UNROLL
      for (i = 0; i < 5; i++) {
        
#pragma HLS PIPELINE II=19
        bc[i] = st[j + i];
      }
      for (i = 0; i < 5; i++) {
        
#pragma HLS PIPELINE II=28 rewind
        st[j + i] ^= ~bc[(i + 1) % 5] & bc[(i + 2) % 5];
      }
    }
    st[0] ^= keccakf_rndc[r];
  }
}
// Initialize the context for SHA3

int sha3_init(sha3_ctx_t *c,int mdlen)
{
  
#pragma UNEXPRESSED
  int i;
  for (i = 0; i < 25; i++) {
    
#pragma UNEXPRESSED
    c -> q[i] = ((uint64_t )0);
  }
  c -> mdlen = mdlen;
  c -> rsiz = 200 - 2 * mdlen;
  c -> pt = 0;
  return 1;
}
// update state with more data

int sha3_update(sha3_ctx_t *c,const void *data,size_t len)
{
  
#pragma UNEXPRESSED
  size_t i;
  int j;
  j = c -> pt;
  for (i = ((size_t )0); i < len; i++) {
    
#pragma UNEXPRESSED
    uint8_t buf = readSingleByte(c -> q,(size_t )j);
    writeSingleByte(c -> q,(size_t )(j++),(uint8_t )(((int )buf) ^ ((int )((const uint8_t *)data)[i])));
    if (j >= c -> rsiz) {
      sha3_keccakf(c -> q);
      j = 0;
    }
  }
  c -> pt = j;
  return 1;
}
// finalize and output a hash

int sha3_final(void *md,sha3_ctx_t *c)
{
  
#pragma HLS INLINE 
  int i;
  writeSingleByte(c -> q,(size_t )(c -> pt),(uint8_t )(((int )(readSingleByte(c -> q,(size_t )(c -> pt)))) ^ 0x06));
  writeSingleByte(c -> q,(size_t )(c -> rsiz - 1),(uint8_t )(((int )(readSingleByte(c -> q,(size_t )(c -> rsiz - 1)))) ^ 0x80));
  sha3_keccakf(c -> q);
  for (i = 0; i < c -> mdlen; i++) {
    
#pragma UNEXPRESSED
    ((uint8_t *)md)[i] = readSingleByte(c -> q,(size_t )i);
  }
  return 1;
}
// compute a SHA-3 hash (md) of given byte length from "in"

void *sha3(const void *in,size_t inlen,void *md,int mdlen)
{
  
#pragma HLS INLINE OFF
  sha3_ctx_t sha3;
  sha3_init(&sha3,mdlen);
  sha3_update(&sha3,in,inlen);
  sha3_final(md,&sha3);
  return md;
}

void OutlinedFunction0(int OrkaParam0,char (*OrkaParam1)[32],char (*OrkaParam2)[32])
{
  
#pragma HLS INLINE OFF
;
  int param_pack_temp_variable0 = (int )OrkaParam0;
  char (*param_pack_temp_variable1)[32] = (char (*)[32])OrkaParam1;
  char (*param_pack_temp_variable2)[32] = (char (*)[32])OrkaParam2;
{
{
      char buf1[32];
      char buf2[32];
      mov(buf1, *param_pack_temp_variable1,(size_t )32);
      for (int i = 0; i < param_pack_temp_variable0; i++) {
        
#pragma UNEXPRESSED
        sha3((const void *)buf1,(size_t )32,(void *)buf2,32);
        mov(buf1,buf2,(size_t )32);
      }
      mov( *param_pack_temp_variable2,buf1,(size_t )32);
    }
  }
}

