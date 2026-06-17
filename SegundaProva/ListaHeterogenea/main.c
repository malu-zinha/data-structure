#include <stdio.h>
#include "lista-hetero.h"

/* Compile com:  gcc main.c lista-hetero.c -o lista
   Execute com:  ./lista                           */
int main(){

    Lista *l = cria_lista();

    printf("Lista vazia? %s\n", vazia(l) ? "sim" : "nao");

    // inserindo tipos DIFERENTES na mesma lista
    printf("\nInserindo um int (42), um float (3.14) e um char ('A')...\n");
    insere_int(l, 42);
    insere_float(l, 3.14);
    insere_char(l, 'A');

    // misturando mais ainda
    insere_int(l, 7);
    insere_char(l, 'z');

    printf("Lista vazia agora? %s\n", vazia(l) ? "sim" : "nao");

    // a impressao mostra cada no com seu tipo e valor corretos
    printf("\nConteudo da lista (cada no sabe o proprio tipo):\n");
    imprime(l);

    libera_lista(l);
    printf("\nLista liberada. Fim.\n");

    return 0;
}
