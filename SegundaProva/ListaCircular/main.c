#include <stdio.h>
#include "lista-circular.h"

/* Imprime a lista circular.
   Usa do-while e para ao "dar a volta" (voltar ao inicio). */
void imprime(Lista *l){

    printf("  lista: [ ");

    if(!vazia(l)){
        No *aux = l->inicio;
        do {
            printf("%d ", aux->val);
            aux = aux->next;
        } while(aux != l->inicio);
    }

    printf("]\n");
}

/* Demonstra que a lista realmente "dá a volta": a partir do inicio,
   damos N+2 passos seguindo o next e vemos os valores se repetirem. */
void mostra_circularidade(Lista *l, int passos){

    if(vazia(l)) return;

    printf("  dando %d passos a partir do inicio: ", passos);
    No *aux = l->inicio;
    for(int i = 0; i < passos; i++){
        printf("%d ", aux->val);
        aux = aux->next; // nunca chega a NULL: volta para o comeco
    }
    printf("\n");
}

/* Compile com:  gcc main.c lista-circular.c -o lista
   Execute com:  ./lista                             */
int main(){

    Lista *l = cria_lista();

    printf("Lista vazia? %s\n", vazia(l) ? "sim" : "nao");

    printf("\nInserindo no fim 10, 20 e 30...\n");
    insere_fim(l, 10);
    insere_fim(l, 20);
    insere_fim(l, 30);

    printf("Inserindo 5 no inicio...\n");
    insere_inicio(l, 5);

    imprime(l);
    printf("Tamanho: %d\n", tamanho(l));

    // a lista tem 4 elementos; 6 passos mostram a volta acontecendo
    printf("\nProva da circularidade:\n");
    mostra_circularidade(l, 6);

    printf("\nRemovendo do inicio...\n");
    remove_inicio(l); // tira o 5
    imprime(l);
    printf("Tamanho: %d\n", tamanho(l));

    libera_lista(l);
    printf("\nLista liberada. Fim.\n");

    return 0;
}
