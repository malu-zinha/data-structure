"""
DEQUE (fila de duas pontas / Double-Ended Queue)
================================================

- Permite inserir e remover nas DUAS pontas: início e fim.
- Generaliza pilha e fila:
    * usando só uma ponta                 -> vira PILHA;
    * entrando por uma e saindo pela outra -> vira FILA.
- Implementado com lista DUPLAMENTE encadeada, para remover do fim em O(1).

    None <- [10] <-> [20] <-> [30] -> None
           inicio            fim

Complexidade: todas as operações nas pontas são O(1).

Pega-ratão
----------
- Com lista simplesmente encadeada, remover do fim seria O(n) (precisa do
  penúltimo). Por isso usamos o ponteiro `anterior`.
- Em Python de verdade use `collections.deque`, que é exatamente isto.
"""


class No:
    def __init__(self, valor):
        self.valor = valor
        self.anterior = None
        self.proximo = None


class Deque:
    def __init__(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def vazio(self):
        return self._tamanho == 0

    def tamanho(self):
        return self._tamanho

    def inserir_inicio(self, valor):
        novo = No(valor)
        if self.vazio():
            self.inicio = self.fim = novo
        else:
            novo.proximo = self.inicio
            self.inicio.anterior = novo
            self.inicio = novo
        self._tamanho += 1

    def inserir_fim(self, valor):
        novo = No(valor)
        if self.vazio():
            self.inicio = self.fim = novo
        else:
            novo.anterior = self.fim
            self.fim.proximo = novo
            self.fim = novo
        self._tamanho += 1

    def remover_inicio(self):
        if self.vazio():
            raise IndexError("Deque vazio")
        no = self.inicio
        self.inicio = no.proximo
        if self.inicio is None:
            self.fim = None
        else:
            self.inicio.anterior = None
        self._tamanho -= 1
        return no.valor

    def remover_fim(self):
        if self.vazio():
            raise IndexError("Deque vazio")
        no = self.fim
        self.fim = no.anterior
        if self.fim is None:
            self.inicio = None
        else:
            self.fim.proximo = None
        self._tamanho -= 1
        return no.valor

    def primeiro(self):
        if self.vazio():
            raise IndexError("Deque vazio")
        return self.inicio.valor

    def ultimo(self):
        if self.vazio():
            raise IndexError("Deque vazio")
        return self.fim.valor

    def limpar(self):
        self.inicio = self.fim = None
        self._tamanho = 0

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        atual = self.inicio
        while atual is not None:
            yield atual.valor
            atual = atual.proximo

    def __str__(self):
        return "início <-> [" + ", ".join(str(v) for v in self) + "] <-> fim"


if __name__ == "__main__":
    deque = Deque()
    deque.inserir_fim(20)
    deque.inserir_fim(30)
    deque.inserir_inicio(10)
    deque.inserir_inicio(5)
    print("Deque:", deque)
    print("Removido do início:", deque.remover_inicio(), "| removido do fim:", deque.remover_fim())
    print("Deque:", deque, "| primeiro:", deque.primeiro(), "| último:", deque.ultimo())
