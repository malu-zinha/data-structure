#include "fila.h"

/* Cria uma fila vazia.
   Aloca a struct Fila e deixa inicio e fim apontando para NULL,
   ou seja, sem nenhum elemento ainda. */
Fila *cria_fila(){

    Fila *f = (Fila *) malloc(sizeof(Fila));

    if(!f) return NULL; // se a malloc falhou, devolve NULL

    f->inicio = NULL;
    f->fim = NULL;

    return f;
}

/* Verifica se a fila está vazia.
   Se não há inicio, a fila está vazia. Retorna 1 ou 0. */
int vazia(Fila *f){

    if(f->inicio == NULL) return 1;

    return 0;
}

/* ENFILEIRAR (enqueue): coloca um novo valor no FIM da fila.
   Cria um nó (que sempre será o último, então next = NULL) e o
   liga no fim atual. Se a fila estava vazia, esse nó é, ao mesmo
   tempo, o inicio e o fim. */
int enfileirar(Fila *f, int dado){

    No *novo = (No*) malloc(sizeof(No));

    if(!novo) return 0; // não conseguiu criar o nó

    novo->val = dado;
    novo->next = NULL; // por ser o último, não aponta para ninguém

    if(vazia(f)){
        f->inicio = novo; // único elemento é inicio e fim
        f->fim = novo;
    } else {
        f->fim->next = novo; // antigo último aponta para o novo
        f->fim = novo;       // o novo passa a ser o último
    }

    return 1;
}

/* DESENFILEIRAR (dequeue): remove o valor do INICIO da fila.
   O valor removido é devolvido pelo ponteiro "dado". O inicio
   passa a ser o segundo elemento. Se a fila ficou vazia, o fim
   também precisa virar NULL. Por isso é FIFO: o primeiro que
   entrou é o primeiro a sair. */
int desenfileirar(Fila *f, int *dado){

    if(vazia(f)) return 0; // não há o que remover

    No *removido = f->inicio; // guarda o primeiro nó
    *dado = removido->val;    // devolve o valor para quem chamou

    f->inicio = removido->next; // inicio passa a ser o próximo

    if(f->inicio == NULL){ // se esvaziou, o fim também vira NULL
        f->fim = NULL;
    }

    free(removido); // libera a memória do nó removido

    return 1;
}

/* DESTRÓI a fila: libera TODA a memória usada.
   Percorre nó por nó dando free, guardando o próximo antes de
   liberar o atual, e no fim libera a struct Fila. */
void destroi_fila(Fila *f){

    if(f == NULL) return;

    No *aux = f->inicio;

    while(aux != NULL){

        No *proximo = aux->next; // guarda o próximo ANTES de liberar
        free(aux);               // libera o nó atual
        aux = proximo;           // avança
    }

    free(f); // por fim, libera a própria estrutura da fila
}
