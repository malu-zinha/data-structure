#include <stdio.h>
#include "lista-encadeada.h"

/* Funcao auxiliar para imprimir a lista do inicio ao fim.
   (Nao faz parte do TAD, e so para visualizarmos o teste.) */
void imprime(Lista *l){

    No *aux = l->inicio;

    printf("  lista: [ ");
    while(aux != NULL){
        printf("%d ", aux->val);
        aux = aux->next;
    }
    printf("]\n");
}

/* Programa de teste da Lista Encadeada.
   Compile com:  gcc main.c lista-encadeada.c -o lista
   Execute com:  ./lista                              */
int main(){

    Lista *l = cria_lista();

    printf("Lista vazia? %s\n", vazia(l) ? "sim" : "nao");
    printf("Tamanho inicial: %d\n", tamanho(l));

    // inserindo no inicio: 10, depois 20, depois 30
    // como cada um entra na frente, a ordem fica 30, 20, 10
    printf("\nInserindo no inicio 10, 20 e 30...\n");
    insere_inicio(l, 10);
    insere_inicio(l, 20);
    insere_inicio(l, 30);
    imprime(l);

    printf("Lista vazia agora? %s\n", vazia(l) ? "sim" : "nao");
    printf("Tamanho: %d\n", tamanho(l));

    // inserindo o 99 na posicao 1 (entre o 30 e o 20)
    printf("\nInserindo 99 na posicao 1...\n");
    insere_meio(l, 1, 99);
    imprime(l);

    // tentando inserir numa posicao que nao existe
    printf("\nTentando inserir na posicao 50 (invalida): %s\n",
           insere_meio(l, 50, 7) ? "inseriu" : "posicao inexistente");

    // removendo do inicio duas vezes
    printf("\nRemovendo do inicio 2 vezes...\n");
    remove_inicio(l);
    remove_inicio(l);
    imprime(l);
    printf("Tamanho: %d\n", tamanho(l));

    libera_lista(l); // libera toda a memoria
    printf("\nLista liberada. Fim.\n");

    return 0;
}
