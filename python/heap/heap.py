r"""
HEAP BINÁRIO (heap mínimo e heap máximo) + HEAPSORT
===================================================

- É uma árvore binária COMPLETA (todos os níveis cheios, exceto talvez o último,
  preenchido da esquerda para a direita) guardada num VETOR, sem ponteiros.
- Propriedade do heap MÍNIMO: todo pai é <= seus filhos -> o menor está na raiz.
  (No heap MÁXIMO: todo pai é >= seus filhos -> o maior está na raiz.)

           1                 vetor: [1, 3, 2, 7, 4, 5]
         /   \                       0  1  2  3  4  5
        3     2
       / \   /             pai(i)             = (i - 1) // 2
      7   4 5              filho_esquerdo(i)  = 2*i + 1
                           filho_direito(i)   = 2*i + 2

Operações
---------
- inserir: coloca no fim do vetor e SOBE (troca com o pai enquanto for menor).
- remover: tira a raiz, põe o último elemento no lugar e DESCE (troca com o
  menor filho enquanto for maior que ele).
- construir (heapify): chama DESCER de trás para frente a partir do último pai.

Complexidade
------------
| operação          | custo      |
|-------------------|------------|
| topo              | O(1)       |
| inserir / remover | O(log n)   |
| construir         | O(n)       |  (sim, O(n), não O(n log n))
| heapsort          | O(n log n) |

Pega-ratão
----------
- Heap NÃO é árvore de busca: não dá para buscar um valor qualquer em O(log n).
- Em Python de verdade use o módulo `heapq` (heap mínimo sobre uma list).
"""


class HeapMinimo:
    def __init__(self, valores=()):
        self._dados = list(valores)
        self._construir()

    # Fórmulas de navegação no vetor -----------------------------------
    @staticmethod
    def _pai(i):
        return (i - 1) // 2

    @staticmethod
    def _esquerdo(i):
        return 2 * i + 1

    @staticmethod
    def _direito(i):
        return 2 * i + 2

    def _vem_antes(self, a, b):
        """Critério de ordem. No heap mínimo, o menor vem antes."""
        return a < b

    # Consultas ---------------------------------------------------------
    def vazio(self):
        return len(self._dados) == 0

    def tamanho(self):
        return len(self._dados)

    def topo(self):
        if self.vazio():
            raise IndexError("Heap vazio")
        return self._dados[0]

    # Operações ---------------------------------------------------------
    def inserir(self, valor):
        self._dados.append(valor)          # entra na primeira vaga livre
        self._subir(len(self._dados) - 1)  # e sobe até o lugar certo

    def remover(self):
        """Remove e devolve o topo (o menor, no heap mínimo)."""
        if self.vazio():
            raise IndexError("Heap vazio")
        topo = self._dados[0]
        ultimo = self._dados.pop()
        if self._dados:
            self._dados[0] = ultimo  # o último vai para a raiz
            self._descer(0)          # e desce até o lugar certo
        return topo

    def _subir(self, i):
        while i > 0:
            pai = self._pai(i)
            if not self._vem_antes(self._dados[i], self._dados[pai]):
                break  # já está certo em relação ao pai
            self._trocar(i, pai)
            i = pai

    def _descer(self, i):
        n = len(self._dados)
        while True:
            escolhido = i
            esquerdo, direito = self._esquerdo(i), self._direito(i)
            if esquerdo < n and self._vem_antes(self._dados[esquerdo], self._dados[escolhido]):
                escolhido = esquerdo
            if direito < n and self._vem_antes(self._dados[direito], self._dados[escolhido]):
                escolhido = direito
            if escolhido == i:
                break  # nenhum filho deveria estar acima dele
            self._trocar(i, escolhido)
            i = escolhido

    def _construir(self):
        """Heapify: as folhas já são heaps; arruma de baixo para cima."""
        for i in range(len(self._dados) // 2 - 1, -1, -1):
            self._descer(i)

    def _trocar(self, i, j):
        self._dados[i], self._dados[j] = self._dados[j], self._dados[i]

    def eh_valido(self):
        """Confere a propriedade do heap em todos os pais."""
        for i in range(1, len(self._dados)):
            if self._vem_antes(self._dados[i], self._dados[self._pai(i)]):
                return False
        return True

    def __len__(self):
        return len(self._dados)

    def __iter__(self):
        """Percorre o VETOR interno (ordem do heap, não ordenada)."""
        return iter(self._dados)

    def __str__(self):
        return str(self._dados)


class HeapMaximo(HeapMinimo):
    def _vem_antes(self, a, b):
        return a > b  # só muda o critério: o maior vem antes


def heapsort(vetor):
    """Ordena `vetor` NO PRÓPRIO VETOR, em ordem crescente -> O(n log n).

    1. Transforma o vetor em heap MÁXIMO.
    2. Repete: troca a raiz (o maior) com o último da parte não ordenada,
       diminui essa parte em 1 e desce a nova raiz.
    """
    n = len(vetor)

    def descer(i, tamanho):
        while True:
            maior = i
            esquerdo, direito = 2 * i + 1, 2 * i + 2
            if esquerdo < tamanho and vetor[esquerdo] > vetor[maior]:
                maior = esquerdo
            if direito < tamanho and vetor[direito] > vetor[maior]:
                maior = direito
            if maior == i:
                return
            vetor[i], vetor[maior] = vetor[maior], vetor[i]
            i = maior

    for i in range(n // 2 - 1, -1, -1):
        descer(i, n)

    for fim in range(n - 1, 0, -1):
        vetor[0], vetor[fim] = vetor[fim], vetor[0]  # o maior vai para o final
        descer(0, fim)                               # arruma o resto
    return vetor


if __name__ == "__main__":
    heap = HeapMinimo()
    for valor in (7, 3, 9, 1, 4, 2):
        heap.inserir(valor)
        print(f"inseriu {valor}: vetor = {heap}")
    print("Removendo em ordem:", [heap.remover() for _ in range(len(heap))])

    maximo = HeapMaximo([7, 3, 9, 1, 4, 2])
    print("Heap máximo construído com heapify:", maximo, "| topo:", maximo.topo())
    print("heapsort([5, 2, 9, 1, 5, 6]) =", heapsort([5, 2, 9, 1, 5, 6]))
