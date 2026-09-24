"""
LISTA SIMPLESMENTE ENCADEADA
============================

- É uma sequência de NÓS. Cada nó guarda um valor e a referência para o
  PRÓXIMO nó. O último nó aponta para None (o NULL do C).
- A lista só precisa saber onde está o primeiro nó (`inicio`, a "cabeça").
- Não usa memória contígua: cada nó é criado separadamente e "ligado" ao próximo.

      inicio
        |
        v
      [10|•]--> [20|•]--> [30|None]

Complexidade
------------
| operação                          | custo |
|-----------------------------------|-------|
| inserir / remover no início       | O(1)  |
| inserir / remover no fim          | O(n)  |  (precisa andar até o último)
| inserir / remover em uma posição  | O(n)  |
| obter / buscar                    | O(n)  |  (acesso é sequencial)

Pega-ratão
----------
- Não existe `lista[i]` direto: para chegar na posição i é preciso andar i nós.
- Para inserir/remover no meio, pare no nó ANTERIOR à posição.
- Ao inverter, guarde o `proximo` ANTES de virar a seta, senão perde o resto.
- Posições começam em 0.
"""


class No:
    """Nó da lista: guarda o valor e a referência para o próximo nó."""

    def __init__(self, valor, proximo=None):
        self.valor = valor
        self.proximo = proximo


class ListaEncadeada:
    def __init__(self):
        self.inicio = None  # cabeça da lista (None = lista vazia)
        self._tamanho = 0   # contador para tamanho() ser O(1)

    # ------------------------------------------------------------------
    # Consultas
    # ------------------------------------------------------------------
    def vazia(self):
        return self.inicio is None

    def tamanho(self):
        return self._tamanho

    def obter(self, pos):
        return self._no_na_posicao(pos).valor

    def alterar(self, pos, valor):
        self._no_na_posicao(pos).valor = valor

    def buscar(self, valor):
        """Devolve a posição da primeira ocorrência de `valor` ou -1."""
        atual = self.inicio
        pos = 0
        while atual is not None:
            if atual.valor == valor:
                return pos
            atual = atual.proximo
            pos += 1
        return -1

    # ------------------------------------------------------------------
    # Inserção
    # ------------------------------------------------------------------
    def inserir_inicio(self, valor):
        # O novo nó aponta para o antigo primeiro e vira a nova cabeça.
        self.inicio = No(valor, self.inicio)
        self._tamanho += 1

    def inserir_fim(self, valor):
        novo = No(valor)
        if self.vazia():
            self.inicio = novo
        else:
            atual = self.inicio
            while atual.proximo is not None:  # anda até o último nó
                atual = atual.proximo
            atual.proximo = novo
        self._tamanho += 1

    def inserir_posicao(self, pos, valor):
        """Insere na posição `pos` (0 = início, tamanho = fim)."""
        if pos < 0 or pos > self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            self.inserir_inicio(valor)
            return
        anterior = self._no_na_posicao(pos - 1)
        # o novo aponta para quem estava na posição, e o anterior aponta pro novo
        anterior.proximo = No(valor, anterior.proximo)
        self._tamanho += 1

    # ------------------------------------------------------------------
    # Remoção
    # ------------------------------------------------------------------
    def remover_inicio(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        removido = self.inicio
        self.inicio = removido.proximo  # a cabeça passa a ser o segundo nó
        self._tamanho -= 1
        return removido.valor

    def remover_fim(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        if self.inicio.proximo is None:  # só um elemento
            return self.remover_inicio()
        anterior = self.inicio
        while anterior.proximo.proximo is not None:  # para no PENÚLTIMO
            anterior = anterior.proximo
        valor = anterior.proximo.valor
        anterior.proximo = None
        self._tamanho -= 1
        return valor

    def remover_posicao(self, pos):
        if self.vazia():
            raise IndexError("Lista vazia")
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            return self.remover_inicio()
        anterior = self._no_na_posicao(pos - 1)
        removido = anterior.proximo
        anterior.proximo = removido.proximo  # "pula" o nó removido
        self._tamanho -= 1
        return removido.valor

    def remover_valor(self, valor):
        """Remove a primeira ocorrência de `valor`. Devolve True se removeu."""
        anterior = None
        atual = self.inicio
        while atual is not None:
            if atual.valor == valor:
                if anterior is None:          # era o primeiro
                    self.inicio = atual.proximo
                else:
                    anterior.proximo = atual.proximo
                self._tamanho -= 1
                return True
            anterior = atual
            atual = atual.proximo
        return False

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def inverter(self):
        """Inverte a lista mudando só as setas (sem criar nós) -> O(n)."""
        anterior = None
        atual = self.inicio
        while atual is not None:
            proximo = atual.proximo   # 1. guarda o resto da lista
            atual.proximo = anterior  # 2. vira a seta para trás
            anterior = atual          # 3. anda os dois ponteiros
            atual = proximo
        self.inicio = anterior        # o antigo último vira a cabeça

    def limpar(self):
        # Em Python o coletor de lixo libera os nós (em C seria um free por nó).
        self.inicio = None
        self._tamanho = 0

    def _no_na_posicao(self, pos):
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        atual = self.inicio
        for _ in range(pos):
            atual = atual.proximo
        return atual

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        atual = self.inicio
        while atual is not None:
            yield atual.valor
            atual = atual.proximo

    def __contains__(self, valor):
        return self.buscar(valor) != -1

    def __str__(self):
        if self.vazia():
            return "None"
        return " -> ".join(str(v) for v in self) + " -> None"


if __name__ == "__main__":
    lista = ListaEncadeada()
    lista.inserir_fim(20)
    lista.inserir_fim(30)
    lista.inserir_inicio(10)
    lista.inserir_posicao(3, 40)
    print("Lista:", lista, "| tamanho:", len(lista))
    print("Posição do 30:", lista.buscar(30))
    print("Elemento na posição 1:", lista.obter(1))
    print("Removido do início:", lista.remover_inicio(), "->", lista)
    print("Removido do fim:", lista.remover_fim(), "->", lista)
    lista.inserir_fim(50)
    lista.inserir_fim(60)
    print("Removeu o 50?", lista.remover_valor(50), "->", lista)
    lista.inverter()
    print("Invertida:", lista)
