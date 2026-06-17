#include <stdio.h>
#include "pilha.h"

/* Programa de teste da Pilha (LIFO).
   Compile com:  gcc main.c pilha.c -o pilha
   Execute com:  ./pilha                  */
int main(){

    Pilha *p = cria_pilha();

    printf("Pilha vazia? %s\n", vazia(p) ? "sim" : "nao");

    // empilhando 10, 20 e 30 (30 fica no topo)
    printf("\nEmpilhando 10, 20 e 30...\n");
    empilhar(p, 10);
    empilhar(p, 20);
    empilhar(p, 30);

    printf("Pilha vazia agora? %s\n", vazia(p) ? "sim" : "nao");

    // desempilhando tudo: como e LIFO, sai 30, depois 20, depois 10
    printf("\nDesempilhando (ordem LIFO):\n");

    int valor;
    while(desempilhar(p, &valor)){
        printf("  saiu: %d\n", valor);
    }

    printf("\nTentando desempilhar de pilha vazia: %s\n",
           desempilhar(p, &valor) ? "removeu" : "nada para remover");

    destroi_pilha(p); // libera toda a memoria
    printf("\nPilha destruida. Fim.\n");

    return 0;
}
