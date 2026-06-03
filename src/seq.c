#include "matrizes.h"

/* Calcula C = A * B de forma sequencial (uma única thread).
   As matrizes são armazenadas em row-major: o elemento [i][j] fica em A[i*n + j].
   Para cada célula C[i][j], percorre a linha i de A e a coluna j de B,
   acumulando os produtos na variável local 'sum' antes de gravar em C. */
void multiply_seq(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++)         /* percorre as linhas de C */
    for (int j = 0; j < n; j++) {       /* percorre as colunas de C */
        double sum = 0.0;
        for (int k = 0; k < n; k++)     /* produto escalar: linha i de A · coluna j de B */
            sum += A[i*n + k] * B[k*n + j];
        C[i*n + j] = sum;               /* grava o resultado na célula */
    }
}
