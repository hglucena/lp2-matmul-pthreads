#ifndef MATRIZES_H
#define MATRIZES_H

#define N          1200
#define NUM_THREADS 12    /* i7-1355U: 10 núcleos físicos, 12 lógicos (2 P-cores c/ HT + 8 E-cores) */
#define NUM_RUNS    6     /* primeira execução é aquecimento e será descartada */

void multiply_seq(const double *A, const double *B, double *C, int n);
void multiply_par(const double *A, const double *B, double *C, int n, int num_threads);

#endif
