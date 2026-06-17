#ifndef LISTA_DUPLA_H
#define LISTA_DUPLA_H

#include <stdlib.h>
#include <stdio.h>

/* Na lista DUPLAMENTE encadeada, cada nó conhece o anterior E o
   próximo. Isso permite andar para os dois lados (frente e trás)
   e remover o último elemento de forma rápida. */
struct no {
    int val;
    struct no* prev; // aponta para o nó ANTERIOR
    struct no* next; // aponta para o PRÓXIMO nó
};

typedef struct no No;

/* Guardamos inicio e fim para acessar as duas pontas em O(1). */
struct lista {
    No *inicio;
    No *fim;
};

typedef struct lista Lista;

/* Protótipos das operações do TAD Lista Duplamente Encadeada. */
Lista *cria_lista();
int vazia(Lista *l);
int tamanho(Lista *l);
int insere_inicio(Lista *l, int dado);
int insere_fim(Lista *l, int dado);
int remove_inicio(Lista *l);
int remove_fim(Lista *l);
void libera_lista(Lista *l);

#endif
