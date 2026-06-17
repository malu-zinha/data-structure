#include "lista-dupla.h"

/* Cria uma lista vazia: inicio e fim apontando para NULL. */
Lista *cria_lista(){

    Lista *l = (Lista *) malloc(sizeof(Lista));

    if(!l) return NULL;

    l->inicio = NULL;
    l->fim = NULL;

    return l;
}

/* Cria um nó isolado, com prev e next em NULL. */
No *cria_no(int dado){

    No *n = (No*) malloc(sizeof(No));

    if(!n) return NULL;

    n->val = dado;
    n->prev = NULL;
    n->next = NULL;

    return n;
}

/* Retorna 1 se a lista estiver vazia, 0 caso contrário. */
int vazia(Lista *l){

    if(l->inicio == NULL) return 1;

    return 0;
}

/* Conta os nós percorrendo do inicio ao fim. */
int tamanho(Lista *l){

    No *aux = l->inicio;
    int tam = 0;

    while(aux != NULL){
        tam++;
        aux = aux->next;
    }

    return tam;
}

/* Insere no INÍCIO.
   O novo nó vira o primeiro: seu next aponta para o antigo inicio
   e o prev do antigo inicio passa a apontar para o novo. */
int insere_inicio(Lista *l, int dado){

    No *novo = cria_no(dado);
    if(!novo) return 0;

    novo->next = l->inicio;

    if(vazia(l)){
        l->fim = novo; // se estava vazia, novo tambem e o fim
    } else {
        l->inicio->prev = novo; // antigo primeiro aponta de volta
    }

    l->inicio = novo; // novo passa a ser o inicio
    return 1;
}

/* Insere no FIM.
   O novo nó vira o último: seu prev aponta para o antigo fim
   e o next do antigo fim passa a apontar para o novo. */
int insere_fim(Lista *l, int dado){

    No *novo = cria_no(dado);
    if(!novo) return 0;

    novo->prev = l->fim;

    if(vazia(l)){
        l->inicio = novo; // se estava vazia, novo tambem e o inicio
    } else {
        l->fim->next = novo; // antigo ultimo aponta para o novo
    }

    l->fim = novo; // novo passa a ser o fim
    return 1;
}

/* Remove o nó do INÍCIO.
   O inicio passa a ser o segundo nó, cujo prev volta a ser NULL.
   Se a lista ficou vazia, o fim também vira NULL. */
int remove_inicio(Lista *l){

    if(vazia(l)) return 0;

    No *removido = l->inicio;
    l->inicio = removido->next;

    if(l->inicio == NULL){
        l->fim = NULL; // ficou vazia
    } else {
        l->inicio->prev = NULL; // novo primeiro nao tem anterior
    }

    free(removido);
    return 1;
}

/* Remove o nó do FIM.
   Aqui o "duplamente" brilha: como o fim conhece o seu prev,
   conseguimos voltar um nó sem precisar percorrer a lista toda. */
int remove_fim(Lista *l){

    if(vazia(l)) return 0;

    No *removido = l->fim;
    l->fim = removido->prev;

    if(l->fim == NULL){
        l->inicio = NULL; // ficou vazia
    } else {
        l->fim->next = NULL; // novo ultimo nao tem proximo
    }

    free(removido);
    return 1;
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
