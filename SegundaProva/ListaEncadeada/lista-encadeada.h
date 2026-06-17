#ifndef LISTAENCADEADA_H
#define LISTAENCADEADA_H

#include <stdlib.h>
#include <stdio.h>

struct no {
    int val;
    struct no* next;
};

typedef struct no No;

struct lista{
    
    No *inicio;
};

typedef struct lista Lista;

/* Protótipos das operações do TAD Lista Encadeada.
   Aqui dizemos O QUE a lista faz; o COMO está no .c */
Lista *cria_lista();
No *cria_no(int dado);
int vazia(Lista *l);
int tamanho(Lista *l);
int insere_inicio(Lista *l, int dado);
int insere_meio(Lista *l, int pos, int dado);
int remove_inicio(Lista *l);
void libera_lista(Lista *l);

#endif