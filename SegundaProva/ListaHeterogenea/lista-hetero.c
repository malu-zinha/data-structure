#include "lista-hetero.h"

/* Cria uma lista vazia. */
Lista *cria_lista(){

    Lista *l = (Lista *) malloc(sizeof(Lista));

    if(!l) return NULL;

    l->inicio = NULL;

    return l;
}

/* Retorna 1 se a lista estiver vazia, 0 caso contrário. */
int vazia(Lista *l){

    if(l->inicio == NULL) return 1;

    return 0;
}

/* Função interna que insere um nó já pronto NO FIM da lista.
   Inserir no fim mantém a ordem de inserção, o que deixa a
   impressão mais intuitiva. As funções insere_* abaixo a reusam. */
int insere_no(Lista *l, No *novo){

    if(!novo) return 0;

    novo->next = NULL;

    if(vazia(l)){
        l->inicio = novo; // primeiro elemento da lista
        return 1;
    }

    // percorre ate o ultimo no e engata o novo la
    No *aux = l->inicio;
    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = novo;

    return 1;
}

/* Insere um INTEIRO: marca o tipo como INTEIRO e usa o campo .i */
int insere_int(Lista *l, int dado){

    No *novo = (No*) malloc(sizeof(No));
    if(!novo) return 0;

    novo->tipo = INTEIRO;
    novo->valor.i = dado;

    return insere_no(l, novo);
}

/* Insere um REAL: marca o tipo como REAL e usa o campo .f */
int insere_float(Lista *l, float dado){

    No *novo = (No*) malloc(sizeof(No));
    if(!novo) return 0;

    novo->tipo = REAL;
    novo->valor.f = dado;

    return insere_no(l, novo);
}

/* Insere um CARACTERE: marca o tipo como CARACTERE e usa o campo .c */
int insere_char(Lista *l, char dado){

    No *novo = (No*) malloc(sizeof(No));
    if(!novo) return 0;

    novo->tipo = CARACTERE;
    novo->valor.c = dado;

    return insere_no(l, novo);
}

/* Imprime a lista.
   Aqui está o ponto-chave da lista heterogênea: olhamos a
   "etiqueta" (tipo) de cada nó para saber QUAL campo da union ler
   e como formatar a saída. */
void imprime(Lista *l){

    No *aux = l->inicio;

    printf("  lista: [ ");
    while(aux != NULL){

        switch(aux->tipo){
            case INTEIRO:
                printf("int:%d ", aux->valor.i);
                break;
            case REAL:
                printf("float:%.2f ", aux->valor.f);
                break;
            case CARACTERE:
                printf("char:%c ", aux->valor.c);
                break;
        }

        aux = aux->next;
    }
    printf("]\n");
}

/* Libera todos os nós e depois a struct Lista. */
void libera_lista(Lista *l){

    if(l == NULL) return;

    No *aux = l->inicio;

    while(aux != NULL){
        No *proximo = aux->next;
        free(aux);
        aux = proximo;
    }

    free(l);
}
