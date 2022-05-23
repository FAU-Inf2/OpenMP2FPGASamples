#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// #define WITH_CACHE_COUNT

int linearize(int x, int y, int bndx, int bndy) {
  if (x >= bndx || y >= bndy)
    return -1;
  return bndy * x + y;
}

void fill_rand(double *data, int bndx, int bndy) {
  for (int i = 0; i < bndx; i++) {
    for (int j = 0; j < bndy; j++) {
      data[linearize(i, j, bndx, bndy)] = ((double)(rand() % 10000)) / 10000;
    }
  }
}

int kernel_cache_misses = 0;
int kernel_cache_hits = 0;

#define KERNEL_CACHE_SIZE 64
double kernel_cache(double *k, int x, int y, const int bndX, const int bndY) {
  // #pragma HLS INLINE ON
  static double cache[KERNEL_CACHE_SIZE] = {0};
  static int offsetInCache = -1;
  int location = linearize(x, y, bndX, bndY);
  int lower = (location / KERNEL_CACHE_SIZE) * sizeof(double);
  uint8_t index = (uint8_t)location & 0x3F;
  int inCache = offsetInCache == lower;
  if (inCache) {
#ifdef WITH_CACHE_COUNT
    kernel_cache_hits++;
#endif
    return cache[index];
  }
  int offset = (location / KERNEL_CACHE_SIZE) * sizeof(double);
  offsetInCache = lower;
  memcpy(cache, k + offset, KERNEL_CACHE_SIZE * sizeof(double));
#ifdef WITH_CACHE_COUNT
  kernel_cache_misses++;
#endif
  return cache[index];
}

int data_cache_misses = 0;
int data_cache_hits = 0;

#define DATA_CACHE_SIZE 64
double data_cache(double *k, int x, int y, const int bndX, const int bndY) {
  // #pragma HLS INLINE ON
  static double cache[DATA_CACHE_SIZE] = {0};

  static int offsetInCache = -1;
  int location = linearize(x, y, bndX, bndY);
  int lower = (location / DATA_CACHE_SIZE) * sizeof(double);
  int inCache = offsetInCache == lower;
  uint8_t index = (uint8_t)location & 0x3F;
  if (inCache) {
#ifdef WITH_CACHE_COUNT
    data_cache_hits++;
#endif
    return cache[index];
  }
  int offset = (location / DATA_CACHE_SIZE) * sizeof(double);
  offsetInCache = lower;
  memcpy(cache, k + offset, DATA_CACHE_SIZE * sizeof(double));
#ifdef WITH_CACHE_COUNT
  data_cache_misses++;
#endif
  return cache[index];
}

void apply_filter_cached(double *kernel, double *data, double *result,
                         const int N, const int M) {
  for (int x = 0; x < N; x++) {
    for (int y = 0; y < N; y++) {
      double weightsum = 0;
      double sum = 0;
      for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
          double weight = kernel_cache(kernel, i, j, M, M);
          double target = data_cache(data, x + i, y + j, N, N);
          if (target < 0)
            continue;
          weightsum += weight;
          sum += weight * target;
        }
      }
      result[linearize(x, y, N, N)] = sum / weightsum;
    }
  }
}

void apply_filter(double *kernel, double *data, double *result, const int N,
                  const int M) {
  for (int x = 0; x < N; x++) {
    for (int y = 0; y < N; y++) {
      double weightsum = 0;
      double sum = 0;
      for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
          double weight = kernel[linearize(i, j, M, M)];
          int target = linearize(x + i, y + j, N, N);
          if (target < 0)
            continue;
          weightsum += weight;
          sum += weight * data[target];
        }
      }
      result[linearize(x, y, N, N)] = sum / weightsum;
    }
  }
}

void check(double *golden, double *toCheck, const int N, const int M,
           const char *logName) {
  FILE *output = fopen(logName, "w");
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int target = linearize(i, j, N, N);
      if (abs(golden[target] - toCheck[target]) > 0.000001) {
        printf("OUTPUT MISMATCH!!!!\nABORTING\n");
        exit(1);
      } else {
        fprintf(output, "%f ", toCheck[target]);
      }
    }
    fprintf(output, "\n");
  }
  fclose(output);
}

int main(int argc, char **argv) {
  int N = 200;
  int M = 10;
  if (argc == 3) { 
  	N = atoi(argv[1]);
  	M = atoi(argv[2]);
  }
  srand(0);
  double *kernel = malloc(sizeof(double) * M * M);
  double *data = malloc(sizeof(double) * N * N);
  double *goldenArr = malloc(sizeof(double) * N * N);
  double *resulta = malloc(sizeof(double) * N * N);
  double *resultb = malloc(sizeof(double) * N * N);
  fill_rand(kernel, M, M);
  fill_rand(data, N, N);

  struct timeval a, b, c, d, e, f, sub_one, sub_two, sub_three;

  // Host Run
  gettimeofday(&a, NULL);
  apply_filter(kernel, data, goldenArr, N, M);
  gettimeofday(&b, NULL);
  timersub(&b, &a, &sub_one);
  printf("CPU filtering complete, Time: %ld.%06ld s\n",
         (long int)sub_one.tv_sec, (long int)sub_one.tv_usec);

  // 1. Run
  gettimeofday(&c, NULL);
#pragma omp target map(tofrom                                                  \
                       : data [0:N * N], kernel [0:M * M], resulta [0:N * N])
  { apply_filter(kernel, data, resulta, N, M); }
  gettimeofday(&d, NULL);

  timersub(&d, &c, &sub_two);
  printf("FPGA filtering complete, Time: %ld.%06ld s\n",
         (long int)sub_two.tv_sec, (long int)sub_two.tv_usec);
  check(goldenArr, resulta, N, M, "log_first_fpga_run.txt");
  fprintf(stderr, "FPGA run successful!\n");

#ifdef WITH_CACHE_COUNT
  fprintf(stderr, "Kernel cache misses: %d and cache hits: %d\n",
          kernel_cache_misses, kernel_cache_hits);
  fprintf(stderr, "Data cache misses: %d and cache hits: %d\n",
          data_cache_misses, data_cache_hits);
#endif

  free(kernel);
  free(data);
  free(goldenArr);
  free(resulta);
  free(resultb);
}
