#include "lista-circular.h"

/* Cria uma lista vazia: inicio apontando para NULL. */
Lista *cria_lista(){

    Lista *l = (Lista *) malloc(sizeof(Lista));

    if(!l) return NULL;

    l->inicio = NULL;

    return l;
}

/* Cria um nó isolado. O next começa em NULL e será ajustado
   na hora da inserção (na lista circular ele nunca fica NULL). */
No *cria_no(int dado){

    No *n = (No*) malloc(sizeof(No));

    if(!n) return NULL;

    n->val = dado;
    n->next = NULL;

    return n;
}

/* Retorna 1 se a lista estiver vazia, 0 caso contrário. */
int vazia(Lista *l){

    if(l->inicio == NULL) return 1;

    return 0;
}

/* Encontra o ÚLTIMO nó: aquele cujo next aponta de volta ao inicio.
   Função interna, usada pelas inserções/remoções. */
No *ultimo(Lista *l){

    if(vazia(l)) return NULL;

    No *aux = l->inicio;
    while(aux->next != l->inicio){ // para quando "der a volta"
        aux = aux->next;
    }

    return aux;
}

/* Conta os nós.
   Usamos do-while porque, mesmo com um único nó, precisamos
   contar pelo menos uma vez antes de checar se voltamos ao inicio. */
int tamanho(Lista *l){

    if(vazia(l)) return 0;

    int tam = 0;
    No *aux = l->inicio;

    do {
        tam++;
        aux = aux->next;
    } while(aux != l->inicio); // para ao retornar ao ponto de partida

    return tam;
}

/* Insere no INÍCIO.
   O novo nó passa a ser o primeiro, mas o último precisa continuar
   apontando para ele (para manter o círculo fechado). */
int insere_inicio(Lista *l, int dado){

    No *novo = cria_no(dado);
    if(!novo) return 0;

    if(vazia(l)){
        novo->next = novo;  // sozinho, aponta para si mesmo
        l->inicio = novo;
        return 1;
    }

    No *fim = ultimo(l);
    novo->next = l->inicio; // novo aponta para o antigo primeiro
    fim->next = novo;       // o ultimo fecha o circulo no novo
    l->inicio = novo;       // novo vira o inicio

    return 1;
}

/* Insere no FIM.
   O antigo último passa a apontar para o novo, e o novo fecha o
   círculo apontando de volta para o inicio. O inicio não muda. */
int insere_fim(Lista *l, int dado){

    No *novo = cria_no(dado);
    if(!novo) return 0;

    if(vazia(l)){
        novo->next = novo;
        l->inicio = novo;
        return 1;
    }

    No *fim = ultimo(l);
    fim->next = novo;       // antigo ultimo aponta para o novo
    novo->next = l->inicio; // novo fecha o circulo no inicio

    return 1;
}

/* Remove o nó do INÍCIO.
   O inicio passa a ser o segundo nó, e o último precisa apontar
   para esse novo inicio (senão o círculo se quebra). */
int remove_inicio(Lista *l){

    if(vazia(l)) return 0;

    No *removido = l->inicio;

    if(removido->next == l->inicio){ // só existe um nó
        free(removido);
        l->inicio = NULL;
        return 1;
    }

    No *fim = ultimo(l);
    l->inicio = removido->next; // segundo nó vira o inicio
    fim->next = l->inicio;      // ultimo fecha o circulo no novo inicio
    free(removido);

    return 1;
}

/* Libera todos os nós.
   ATENÇÃO: como a lista é circular, não dá para parar no NULL.
   Guardamos o inicio e percorremos até voltar nele, liberando. */
void libera_lista(Lista *l){

    if(l == NULL) return;

    if(vazia(l)){
        free(l);
        return;
    }

    No *aux = l->inicio;
    No *parada = l->inicio; // ponto de referência para saber a volta

    do {
        No *proximo = aux->next;
        free(aux);
        aux = proximo;
    } while(aux != parada); // para ao retornar ao inicio

    free(l);
}
