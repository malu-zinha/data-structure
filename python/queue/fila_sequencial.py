"""
FILA SEQUENCIAL CIRCULAR (estática, com vetor)
==============================================

- FIFO: First In, First Out -> o PRIMEIRO a entrar é o PRIMEIRO a sair.
- Entra pelo FIM, sai pelo INÍCIO. Pense na fila do banco.
- Com vetor simples, cada saída deixaria um "buraco" no começo. A solução é o
  VETOR CIRCULAR: os índices dão a volta usando o resto da divisão (%).

  capacidade = 5, depois de algumas entradas e saídas:

    índice:   0    1    2    3    4
            [ 40 | 50 |    | 20 | 30 ]
                        ^fim  ^inicio      ordem de saída: 20, 30, 40, 50

  próximo índice = (índice + 1) % capacidade

Complexidade: enfileirar, desenfileirar, frente, vazia e cheia são O(1).

Pega-ratão
----------
- Só com `inicio` e `fim` não dá para diferenciar vazia de cheia (nos dois
  casos inicio == fim). Por isso guardamos também `_tamanho`.
- Esquecer o `% capacidade` faz o índice estourar o vetor.
"""


class FilaSequencial:
    def __init__(self, capacidade=10):
        if capacidade <= 0:
            raise ValueError("A capacidade deve ser positiva")
        self._dados = [None] * capacidade
        self._capacidade = capacidade
        self._inicio = 0   # índice de quem sai primeiro
        self._fim = 0      # índice da próxima posição livre
        self._tamanho = 0

    def vazia(self):
        return self._tamanho == 0

    def cheia(self):
        return self._tamanho == self._capacidade

    def tamanho(self):
        return self._tamanho

    def enfileirar(self, valor):
        """enqueue: entra no fim."""
        if self.cheia():
            raise OverflowError("Fila cheia")
        self._dados[self._fim] = valor
        self._fim = (self._fim + 1) % self._capacidade  # dá a volta no vetor
        self._tamanho += 1

    def desenfileirar(self):
        """dequeue: sai do início."""
        if self.vazia():
            raise IndexError("Fila vazia")
        valor = self._dados[self._inicio]
        self._dados[self._inicio] = None
        self._inicio = (self._inicio + 1) % self._capacidade
        self._tamanho -= 1
        return valor

    def frente(self):
        """Olha quem é o próximo a sair, sem remover."""
        if self.vazia():
            raise IndexError("Fila vazia")
        return self._dados[self._inicio]

    def ultimo(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        return self._dados[(self._fim - 1) % self._capacidade]

    def limpar(self):
        self._dados = [None] * self._capacidade
        self._inicio = self._fim = self._tamanho = 0

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        """Percorre na ordem de saída."""
        for i in range(self._tamanho):
            yield self._dados[(self._inicio + i) % self._capacidade]

    def __str__(self):
        return "início -> [" + ", ".join(str(v) for v in self) + "] <- fim"


if __name__ == "__main__":
    fila = FilaSequencial(capacidade=4)
    for valor in (10, 20, 30, 40):
        fila.enfileirar(valor)
    print("Fila:", fila, "| cheia?", fila.cheia())
    print("Saiu:", fila.desenfileirar(), "| saiu:", fila.desenfileirar())
    fila.enfileirar(50)
    fila.enfileirar(60)  # estas entram no começo do vetor (deu a volta)
    print("Fila:", fila)
    print("Vetor interno:", fila._dados, "| início =", fila._inicio, "| fim =", fila._fim)
    print("Frente:", fila.frente(), "| último:", fila.ultimo())
