#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrizes.h"

/* Calcula a diferença entre dois instantes do relógio monotônico em segundos.
   tv_sec guarda segundos inteiros e tv_nsec os nanossegundos restantes. */
static double diff_sec(struct timespec t0, struct timespec t1) {
    return (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
}

/* checksum = soma de todos os elementos de C; usado como âncora de corretude */
static double checksum(const double *C, int n) {
    double sum = 0.0;
    for (long i = 0; i < (long)n * n; i++)
        sum += C[i];
    return sum;
}

int main(int argc, char *argv[]) {
    /* número de threads: usa argv[1] se fornecido, senão cai no padrão NUM_THREADS de matrizes.h
       exemplo: ./matmul 8   →  roda com 8 threads
                ./matmul     →  roda com NUM_THREADS (definido em matrizes.h) */
    int num_threads = (argc > 1) ? atoi(argv[1]) : NUM_THREADS;
    if (num_threads < 1) num_threads = 1;

    /* --- setup: alocação e preenchimento (fora do cronômetro) ---
       Duas matrizes de entrada A e B, e duas de saída separadas para
       comparar o resultado sequencial (C_seq) com o paralelo (C_par). */
    double *A     = malloc((long)N * N * sizeof(double));
    double *B     = malloc((long)N * N * sizeof(double));
    double *C_seq = malloc((long)N * N * sizeof(double));
    double *C_par = malloc((long)N * N * sizeof(double));
    if (!A || !B || !C_seq || !C_par) {
        fprintf(stderr, "erro: sem memória\n");
        return 1;
    }

    /* semente fixa garante que A e B sejam idênticas em toda execução,
       permitindo comparar resultados entre sequencial e paralelo */
    srand(42);
    for (long i = 0; i < (long)N * N; i++) {
        A[i] = (double)rand() / RAND_MAX;  /* valores entre 0.0 e 1.0 */
        B[i] = (double)rand() / RAND_MAX;
    }

    printf("P1 — Multiplicação de matrizes %d×%d\n", N, N);
    printf("Threads: %d | Execuções: %d (1ª descartada como aquecimento)\n\n",
           num_threads, NUM_RUNS);

    struct timespec t0, t1;
    double times[NUM_RUNS];

    /* --- versão sequencial ---
       Executa NUM_RUNS vezes; a primeira é descartada pois o processador
       ainda está "frio" (cache vazia, branch predictor não treinado). */
    printf("[ Sequencial ]\n");
    for (int i = 0; i < NUM_RUNS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);   /* marca início */
        multiply_seq(A, B, C_seq, N);
        clock_gettime(CLOCK_MONOTONIC, &t1);   /* marca fim */
        times[i] = diff_sec(t0, t1);
        printf("  exec %d: %.4f s%s\n", i + 1, times[i], i == 0 ? " (aquecimento)" : "");
    }
    double sum = 0.0;
    for (int i = 1; i < NUM_RUNS; i++) sum += times[i];  /* ignora exec 0 */
    double t_seq = sum / (NUM_RUNS - 1);
    printf("  T_seq = %.4f s\n\n", t_seq);

    /* --- versão paralela ---
       Mesma metodologia: NUM_RUNS rodadas, descarta a primeira. */
    printf("[ Paralela — %d thread(s) ]\n", num_threads);
    for (int i = 0; i < NUM_RUNS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);   /* marca início (inclui criação e join das threads) */
        multiply_par(A, B, C_par, N, num_threads);
        clock_gettime(CLOCK_MONOTONIC, &t1);   /* marca fim */
        times[i] = diff_sec(t0, t1);
        printf("  exec %d: %.4f s%s\n", i + 1, times[i], i == 0 ? " (aquecimento)" : "");
    }
    sum = 0.0;
    for (int i = 1; i < NUM_RUNS; i++) sum += times[i];  /* ignora exec 0 */
    double t_par = sum / (NUM_RUNS - 1);
    printf("  T_par = %.4f s\n\n", t_par);

    /* --- verificação de corretude (âncora) ---
       Compara o checksum de C_seq com C_par; diferença abaixo de 1e-6
       é tolerada por causa de arredondamento de ponto flutuante. */
    double ck_seq = checksum(C_seq, N);
    double ck_par = checksum(C_par, N);
    double diff   = ck_seq - ck_par;
    if (diff < 0) diff = -diff;
    printf("Checksum seq: %.6e\n", ck_seq);
    printf("Checksum par: %.6e\n", ck_par);
    printf("Ancora:       %s\n\n", diff < 1e-6 ? "OK" : "FALHA");

    /* --- tabela de speedup ---
       speedup = T_seq / T_par; quanto maior, mais eficiente o paralelismo */
    printf("%-12s %-12s %-10s\n", "Threads", "Tempo (s)", "Speedup");
    printf("%-12s %-12.4f %-10s\n", "1 (seq)",   t_seq, "1.00x");
    printf("%-12d %-12.4f %.2fx\n",  num_threads, t_par, t_seq / t_par);

    free(A); free(B); free(C_seq); free(C_par);
    return 0;
}
