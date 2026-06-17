#include "pilha.h"

/* Cria uma pilha vazia.
   Aloca a struct Pilha e deixa o "topo" apontando para NULL,
   ou seja, sem nenhum elemento ainda. */
Pilha *cria_pilha(){

    Pilha *p = (Pilha *) malloc(sizeof(Pilha));

    if(!p) return NULL; // se a malloc falhou, devolve NULL

    p->topo = NULL;

    return p;
}

/* Verifica se a pilha está vazia.
   Retorna 1 (verdadeiro) se o topo for NULL, e 0 caso contrário. */
int vazia(Pilha *p){

    if(p->topo == NULL) return 1;

    return 0;
}

/* EMPILHAR (push): coloca um novo valor no TOPO da pilha.
   Cria um nó, faz ele apontar para o antigo topo e atualiza
   o topo para o novo nó. Por isso a pilha é LIFO: o último a
   entrar fica no topo e será o primeiro a sair. */
int empilhar(Pilha *p, int dado){

    No *novo = (No*) malloc(sizeof(No));

    if(!novo) return 0; // não conseguiu criar o nó

    novo->val = dado;
    novo->next = p->topo; // novo aponta para o antigo topo
    p->topo = novo;       // o topo passa a ser o novo nó

    return 1;
}

/* DESEMPILHAR (pop): remove o valor que está no TOPO.
   O valor removido é devolvido através do ponteiro "dado"
   (por isso passamos um int* ). Em seguida o topo passa a ser
   o nó de baixo e liberamos o nó removido da memória. */
int desempilhar(Pilha *p, int *dado){

    if(vazia(p)) return 0; // não há o que remover

    No *removido = p->topo; // guarda o nó do topo
    *dado = removido->val;  // devolve o valor para quem chamou

    p->topo = removido->next; // topo passa a ser o nó de baixo
    free(removido);           // libera a memória do nó removido

    return 1;
}

/* DESTRÓI a pilha: libera TODA a memória usada.
   Percorre nó por nó dando free, guardando o próximo antes
   de liberar o atual, e no fim libera a struct Pilha. */
void destroi_pilha(Pilha *p){

    if(p == NULL) return;

    No *aux = p->topo;

    while(aux != NULL){

        No *proximo = aux->next; // guarda o próximo ANTES de liberar
        free(aux);               // libera o nó atual
        aux = proximo;           // avança
    }

    free(p); // por fim, libera a própria estrutura da pilha
}
