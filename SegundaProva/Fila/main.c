#include <stdio.h>
#include "fila.h"

/* Programa de teste da Fila (FIFO).
   Compile com:  gcc main.c fila.c -o fila
   Execute com:  ./fila                    */
int main(){

    Fila *f = cria_fila();

    printf("Fila vazia? %s\n", vazia(f) ? "sim" : "nao");

    // enfileirando 10, 20 e 30 (10 fica na frente)
    printf("\nEnfileirando 10, 20 e 30...\n");
    enfileirar(f, 10);
    enfileirar(f, 20);
    enfileirar(f, 30);

    printf("Fila vazia agora? %s\n", vazia(f) ? "sim" : "nao");

    // desenfileirando tudo: como e FIFO, sai 10, depois 20, depois 30
    printf("\nDesenfileirando (ordem FIFO):\n");

    int valor;
    while(desenfileirar(f, &valor)){
        printf("  saiu: %d\n", valor);
    }

    printf("\nTentando desenfileirar de fila vazia: %s\n",
           desenfileirar(f, &valor) ? "removeu" : "nada para remover");

    destroi_fila(f); // libera toda a memoria
    printf("\nFila destruida. Fim.\n");

    return 0;
}
