#include <stdio.h>
#include "lista-dupla.h"

/* Imprime do inicio ao fim (usando o ponteiro next). */
void imprime_frente(Lista *l){
    No *aux = l->inicio;
    printf("  frente -> tras: [ ");
    while(aux != NULL){
        printf("%d ", aux->val);
        aux = aux->next;
    }
    printf("]\n");
}

/* Imprime do fim ao inicio (usando o ponteiro prev).
   So e possivel por ser DUPLAMENTE encadeada! */
void imprime_tras(Lista *l){
    No *aux = l->fim;
    printf("  tras -> frente: [ ");
    while(aux != NULL){
        printf("%d ", aux->val);
        aux = aux->prev;
    }
    printf("]\n");
}

/* Compile com:  gcc main.c lista-dupla.c -o lista
   Execute com:  ./lista                          */
int main(){

    Lista *l = cria_lista();

    printf("Lista vazia? %s\n", vazia(l) ? "sim" : "nao");

    printf("\nInserindo no fim 10, 20 e 30...\n");
    insere_fim(l, 10);
    insere_fim(l, 20);
    insere_fim(l, 30);

    printf("Inserindo 5 no inicio...\n");
    insere_inicio(l, 5);

    imprime_frente(l);
    imprime_tras(l); // prova que da pra andar nos dois sentidos
    printf("Tamanho: %d\n", tamanho(l));

    printf("\nRemovendo do inicio e do fim...\n");
    remove_inicio(l); // tira o 5
    remove_fim(l);    // tira o 30
    imprime_frente(l);
    printf("Tamanho: %d\n", tamanho(l));

    libera_lista(l);
    printf("\nLista liberada. Fim.\n");

    return 0;
}
