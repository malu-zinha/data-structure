#ifndef LISTA_CIRCULAR_H
#define LISTA_CIRCULAR_H

#include <stdlib.h>
#include <stdio.h>

/* Na lista CIRCULAR, o último nó não aponta para NULL: ele aponta
   de volta para o primeiro. Assim a lista "dá a volta" e nunca
   termina de fato — por isso é preciso cuidado nos laços para não
   girar para sempre. */
struct no {
    int val;
    struct no* next;
};

typedef struct no No;

/* Guardamos apenas o inicio. O fim é alcançado andando até o nó
   cujo next volta a ser o inicio. */
struct lista {
    No *inicio;
};

typedef struct lista Lista;

/* Protótipos das operações do TAD Lista Circular. */
Lista *cria_lista();
int vazia(Lista *l);
int tamanho(Lista *l);
int insere_inicio(Lista *l, int dado);
int insere_fim(Lista *l, int dado);
int remove_inicio(Lista *l);
void libera_lista(Lista *l);

#endif
