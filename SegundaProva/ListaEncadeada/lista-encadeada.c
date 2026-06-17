#include "lista-encadeada.h"

/* Cria uma lista vazia.
   Aloca a struct Lista na memória e deixa o ponteiro "inicio"
   apontando para NULL (ou seja, sem nenhum nó dentro ainda). */
Lista *cria_lista(){

    Lista *l = (Lista *) malloc(sizeof(Lista));

    if(!l) return NULL; // se a malloc falhou, devolve NULL

    l->inicio = NULL;

    return l;
}

/* Cria um nó isolado (ainda fora da lista).
   Guarda o valor recebido em "val" e deixa "next" apontando
   para NULL, já que esse nó ainda não está ligado a nenhum outro. */
No *cria_no(int dado){

    No *n = (No*) malloc(sizeof(No));

    if(!n) return NULL; // se a malloc falhou, devolve NULL

    n->val = dado;
    n->next = NULL;

    return n;
}

/* Verifica se a lista está vazia.
   Retorna 1 (verdadeiro) se o inicio for NULL, e 0 caso contrário. */
int vazia(Lista *l){

    if(l->inicio == NULL) return 1;

    return 0;
}

/* Conta quantos nós existem na lista.
   Percorre nó por nó a partir do inicio, incrementando o contador,
   até chegar no fim (quando o ponteiro vira NULL). */
int tamanho(Lista *l){

    if(vazia(l)) return 0;

    No *aux = l->inicio; // ponteiro auxiliar para "andar" pela lista
    int tam = 0;

    while (aux != NULL){

        tam++;
        aux = aux->next; // avança para o próximo nó
    }

    return tam;
}

/* Insere um novo valor no INÍCIO da lista.
   O novo nó passa a apontar para quem era o primeiro,
   e a lista passa a começar pelo novo nó. */
int insere_inicio(Lista *l, int dado){

    No *novo = cria_no(dado);

    if(!novo) return 0; // não conseguiu criar o nó

    novo->next = l->inicio; // novo aponta para o antigo primeiro
    l->inicio = novo;       // a lista agora começa no novo

    return 1;
}

/* Insere um novo valor em uma POSIÇÃO específica (pos).
   Posição 0 = início. Para posições maiores, anda até o nó
   anterior à posição desejada e "encaixa" o novo nó ali. */
int insere_meio(Lista *l, int pos, int dado){

    // posição inválida (negativa)
    if(pos < 0) return 0;

    // inserir na posição 0 é o mesmo que inserir no início
    if(pos == 0) return insere_inicio(l, dado);

    No *novo = cria_no(dado);
    if(!novo) return 0;

    // anda até o nó que fica ANTES da posição onde vamos inserir
    No *aux = l->inicio;
    int i = 0;

    while(i < pos - 1 && aux != NULL){
        aux = aux->next;
        i++;
    }

    // se aux for NULL, a posição não existe na lista
    if(aux == NULL){
        free(novo); // descarta o nó que não vai ser usado
        return 0;
    }

    // encaixa o novo nó entre aux e o próximo de aux
    novo->next = aux->next;
    aux->next = novo;

    return 1;
}

/* Remove o primeiro nó da lista (o do início).
   Guarda o início atual, faz a lista começar pelo segundo nó
   e libera da memória o nó removido. */
int remove_inicio(Lista *l){

    if(vazia(l)) return 0; // nada para remover

    No *removido = l->inicio;   // guarda quem vai sair
    l->inicio = removido->next; // lista passa a começar no segundo nó

    free(removido); // libera a memória do nó removido

    return 1;
}

/* Libera TODA a memória usada pela lista.
   Percorre nó por nó dando free em cada um, guardando antes
   o próximo (senão perderíamos o caminho ao liberar o atual),
   e no fim libera também a struct Lista. */
void libera_lista(Lista *l){

    if(l == NULL) return; // nada para liberar

    No *aux = l->inicio;

    while(aux != NULL){

        No *proximo = aux->next; // guarda o próximo ANTES de liberar
        free(aux);               // libera o nó atual
        aux = proximo;           // avança para o próximo
    }

    free(l); // por fim, libera a própria estrutura da lista
}
