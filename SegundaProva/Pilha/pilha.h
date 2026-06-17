#ifndef PILHA_H
#define PILHA_H

#include <stdlib.h>
#include <stdio.h>

/* Cada nó guarda um valor e aponta para o nó de baixo na pilha. */
struct no {
    int val;
    struct no* next;
};

typedef struct no No;

/* A pilha só precisa saber onde está o TOPO.
   Como é LIFO, tudo entra e sai pelo topo. */
struct pilha {
    No *topo;
};

typedef struct pilha Pilha;

/* Protótipos das operações do TAD Pilha.
   O QUE a pilha faz fica aqui; o COMO está no .c */
Pilha *cria_pilha();
int vazia(Pilha *p);
int empilhar(Pilha *p, int dado);   // push
int desempilhar(Pilha *p, int *dado); // pop
void destroi_pilha(Pilha *p);

#endif
