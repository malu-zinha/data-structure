#ifndef LISTA_HETERO_H
#define LISTA_HETERO_H

#include <stdlib.h>
#include <stdio.h>

/* Lista HETEROGÊNEA: cada nó pode guardar um tipo DIFERENTE de
   dado (aqui: inteiro, real ou caractere). Os truques de C usados
   são o ENUM (para marcar o tipo) e a UNION (para guardar o valor). */

/* O enum identifica QUAL tipo o nó está guardando. */
enum tipo { INTEIRO, REAL, CARACTERE };
typedef enum tipo Tipo;

/* A union reserva espaço para UM valor por vez: ou int, ou float,
   ou char. Diferente de um struct, os campos compartilham a mesma
   memória, então só um deles é válido de cada vez. */
union valor {
    int i;
    float f;
    char c;
};
typedef union valor Valor;

/* O nó junta as duas coisas: a "etiqueta" (tipo) que diz qual campo
   da union está em uso, e o valor em si. */
struct no {
    Tipo tipo;
    Valor valor;
    struct no* next;
};

typedef struct no No;

struct lista {
    No *inicio;
};

typedef struct lista Lista;

/* Protótipos das operações do TAD Lista Heterogênea. */
Lista *cria_lista();
int vazia(Lista *l);
int insere_int(Lista *l, int dado);
int insere_float(Lista *l, float dado);
int insere_char(Lista *l, char dado);
void imprime(Lista *l);
void libera_lista(Lista *l);

#endif
