#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sha3.h"
#define PROBSIZE 32

char parse_to_hex(char in){
  in = in & 0xF;
  if(in < 10) return '0'+in;
  return 'A'+(in-10);
}

void hex_from_bin(char* hex, char* bin, size_t len){
  for(size_t i = 0; i < len; i++){
    hex[2*i] = parse_to_hex(bin[i]>>4);
    hex[2*i+1] = parse_to_hex(bin[i]);
  }
  hex[len*2] = '\0';
}

void mov(char* to, char* from, size_t len){
  for(int i = 0; i < len; i++){
    to[i] = from[i];
  }
}

// main
int main(int argc, char **argv)
{
    int rounds = 10000;
    if(argc >= 2) rounds = atoi(argv[1]);
    srand(0);
    char input[PROBSIZE];
    char input_str[2*PROBSIZE+1];
    for(size_t i = 0; i < PROBSIZE; i++){
      input[i] = (char)rand();
    }
    hex_from_bin(input_str,input,PROBSIZE);
    printf("Hashing:\n%s\n%d times\n",input_str,rounds);

    char res_target[PROBSIZE];
    char res_host[PROBSIZE];

    #pragma omp target map(to : input) map(from : res_target)
    {
      char buf1[PROBSIZE];
      char buf2[PROBSIZE];
      mov(buf1, input, PROBSIZE);
      for(int i = 0; i < rounds; i++){
        sha3(buf1, PROBSIZE, buf2, PROBSIZE);
        mov(buf1,buf2,PROBSIZE);
      }
      mov(res_target,buf1,PROBSIZE);
    }

    {
      char buf1[PROBSIZE];
      char buf2[PROBSIZE];
      mov(buf1, input, PROBSIZE);
      for(int i = 0; i < rounds; i++){
        sha3(buf1, PROBSIZE, buf2, PROBSIZE);
        mov(buf1,buf2,PROBSIZE);
      }
      mov(res_host,buf1,PROBSIZE);
    }

    char res_target_str[2*PROBSIZE+1];
    hex_from_bin(res_target_str,res_target,PROBSIZE);

    char res_host_str[2*PROBSIZE+1];
    hex_from_bin(res_host_str,res_host,PROBSIZE);

    printf("Result target: %s\n", res_target_str);
    printf("Result host:   %s\n", res_host_str);

    int cmp = strncmp(res_host_str,res_target_str,2*PROBSIZE);
    if(!cmp) printf("Success!\n");
    else printf("Failure!\n");
    return cmp;
}
