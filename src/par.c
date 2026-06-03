#include <pthread.h>
#include <stdlib.h>
#include "matrizes.h"

/* Struct de argumentos passada a cada thread.
   Cada thread recebe ponteiros para as matrizes A, B e C,
   e o intervalo de linhas [row_start, row_end) que ela deve calcular. */
typedef struct {
    const double *A;
    const double *B;
    double       *C;
    int           n;
    int           row_start;   /* linha inicial da fatia (inclusivo) */
    int           row_end;     /* linha final da fatia (exclusivo)   */
} Arg;

/* Função executada por cada thread.
   Calcula apenas as linhas [row_start, row_end) de C.
   Como cada thread escreve em linhas diferentes, não há escrita compartilhada
   e portanto não é necessário mutex nem outra sincronização. */
static void *worker(void *p) {
    Arg *a = (Arg *)p;  /* converte o ponteiro genérico de volta para Arg */
    for (int i = a->row_start; i < a->row_end; i++)   /* linhas da fatia desta thread */
        for (int j = 0; j < a->n; j++) {              /* todas as colunas */
            double sum = 0.0;
            for (int k = 0; k < a->n; k++)            /* produto escalar linha i de A · coluna j de B */
                sum += a->A[i * a->n + k] * a->B[k * a->n + j];
            a->C[i * a->n + j] = sum;                 /* resultado gravado diretamente em C */
        }
    return NULL;  /* sem valor de retorno: o resultado já está em C */
}

/* Lança num_threads threads, cada uma calculando um bloco de linhas de C.
   Aguarda todas terminarem com pthread_join antes de retornar.
   Quando esta função retorna, C está completamente preenchida. */
void multiply_par(const double *A, const double *B, double *C, int n, int num_threads) {
    pthread_t *threads    = malloc((size_t)num_threads * sizeof(pthread_t));
    Arg       *args       = malloc((size_t)num_threads * sizeof(Arg));

    /* divide as n linhas igualmente; se não for divisível, as primeiras threads
       ficam com uma linha a mais (distribui o remainder uma a uma) */
    int rows_per_thread = n / num_threads;
    int remainder       = n % num_threads;
    int current_row     = 0;

    for (int t = 0; t < num_threads; t++) {
        /* preenche o struct de argumentos com a fatia desta thread */
        args[t].A         = A;
        args[t].B         = B;
        args[t].C         = C;
        args[t].n         = n;
        args[t].row_start = current_row;
        args[t].row_end   = current_row + rows_per_thread + (t < remainder ? 1 : 0);
        current_row       = args[t].row_end;

        /* cria a thread apontando para worker e passa o struct como argumento */
        pthread_create(&threads[t], NULL, worker, &args[t]);
    }

    /* aguarda cada thread terminar antes de retornar;
       NULL no segundo argumento significa que não precisamos do valor de retorno */
    for (int t = 0; t < num_threads; t++)
        pthread_join(threads[t], NULL);

    free(threads);
    free(args);
}
