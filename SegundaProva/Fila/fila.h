#ifndef FILA_H
#define FILA_H

#include <stdlib.h>
#include <stdio.h>

/* Cada nó guarda um valor e aponta para o próximo da fila. */
struct no {
    int val;
    struct no* next;
};

typedef struct no No;

/* A fila guarda o INICIO (de onde sai) e o FIM (onde entra).
   Como é FIFO, removemos pelo inicio e inserimos pelo fim.
   Guardar o "fim" evita ter que percorrer a fila toda a cada
   inserção. */
struct fila {
    No *inicio;
    No *fim;
};

typedef struct fila Fila;

/* Protótipos das operações do TAD Fila.
   O QUE a fila faz fica aqui; o COMO está no .c */
Fila *cria_fila();
int vazia(Fila *f);
int enfileirar(Fila *f, int dado);    // enqueue (entra no fim)
int desenfileirar(Fila *f, int *dado); // dequeue (sai do inicio)
void destroi_fila(Fila *f);

#endif
