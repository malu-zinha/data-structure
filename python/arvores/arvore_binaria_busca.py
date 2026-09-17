r"""
ÁRVORE BINÁRIA DE BUSCA (ABB / BST)
===================================

Regra: para TODO nó,
  - tudo na subárvore ESQUERDA é MENOR que ele;
  - tudo na subárvore DIREITA é MAIOR que ele.

              50
            /    \
          30      70
         /  \    /  \
       20   40  60   80

- O percurso EM ORDEM sai ORDENADO: 20 30 40 50 60 70 80.
- Mínimo = nó mais à esquerda. Máximo = nó mais à direita.
- Valores repetidos são ignorados nesta implementação.

Remoção (3 casos)
-----------------
1. Nó FOLHA:          simplesmente some.
2. Nó com UM filho:   o filho sobe e ocupa o lugar dele.
3. Nó com DOIS filhos: copia o valor do SUCESSOR (menor da subárvore direita)
                       e remove o sucessor lá embaixo (ele cai no caso 1 ou 2).
                       (Também daria para usar o ANTECESSOR: maior da esquerda.)

Complexidade
------------
| operação                  | caso médio | pior caso |
|---------------------------|------------|-----------|
| buscar / inserir / remover| O(log n)   | O(n)      |
| mínimo / máximo           | O(log n)   | O(n)      |
| percursos                 | O(n)       | O(n)      |

Pega-ratão
----------
- O pior caso acontece quando os valores entram JÁ ORDENADOS: a árvore vira
  uma "lista" torta. É por isso que existem AVL e Rubro-Negra.
- Altura aqui conta ARESTAS (folha = 0, vazia = -1).
"""

from collections import deque


class No:
    def __init__(self, valor):
        self.valor = valor
        self.esquerda = None
        self.direita = None


class ArvoreBinariaBusca:
    def __init__(self, valores=()):
        self.raiz = None
        self._tamanho = 0
        for valor in valores:
            self.inserir(valor)

    def vazia(self):
        return self.raiz is None

    def tamanho(self):
        return self._tamanho

    # ==================================================================
    # Inserção
    # ==================================================================
    def inserir(self, valor):
        """Versão ITERATIVA: desce comparando até achar a vaga. True se inseriu."""
        novo = No(valor)
        if self.raiz is None:
            self.raiz = novo
            self._tamanho += 1
            return True

        atual = self.raiz
        while True:
            if valor == atual.valor:
                return False  # repetido
            if valor < atual.valor:
                if atual.esquerda is None:
                    atual.esquerda = novo
                    break
                atual = atual.esquerda
            else:
                if atual.direita is None:
                    atual.direita = novo
                    break
                atual = atual.direita
        self._tamanho += 1
        return True

    def inserir_recursivo(self, valor):
        """Mesma coisa, na versão RECURSIVA (a mais cobrada em prova)."""
        if self.contem(valor):
            return False
        self.raiz = self._inserir(self.raiz, valor)
        self._tamanho += 1
        return True

    def _inserir(self, no, valor):
        if no is None:
            return No(valor)  # achou a vaga: vira uma folha nova
        if valor < no.valor:
            no.esquerda = self._inserir(no.esquerda, valor)
        else:
            no.direita = self._inserir(no.direita, valor)
        return no

    # ==================================================================
    # Busca
    # ==================================================================
    def buscar(self, valor):
        """Versão RECURSIVA. Devolve o nó ou None."""
        def procurar(no):
            if no is None or no.valor == valor:
                return no
            if valor < no.valor:
                return procurar(no.esquerda)  # só precisa olhar UM lado
            return procurar(no.direita)

        return procurar(self.raiz)

    def buscar_iterativo(self, valor):
        atual = self.raiz
        while atual is not None and atual.valor != valor:
            atual = atual.esquerda if valor < atual.valor else atual.direita
        return atual

    def contem(self, valor):
        return self.buscar_iterativo(valor) is not None

    def minimo(self):
        if self.raiz is None:
            raise ValueError("Árvore vazia")
        return self._minimo_no(self.raiz).valor

    def maximo(self):
        if self.raiz is None:
            raise ValueError("Árvore vazia")
        atual = self.raiz
        while atual.direita is not None:  # sempre para a direita
            atual = atual.direita
        return atual.valor

    @staticmethod
    def _minimo_no(no):
        while no.esquerda is not None:  # sempre para a esquerda
            no = no.esquerda
        return no

    def sucessor(self, valor):
        """Menor valor da árvore que é MAIOR que `valor` (ou None)."""
        candidato = None
        atual = self.raiz
        while atual is not None:
            if valor < atual.valor:
                candidato = atual.valor  # serve, mas pode ter um menor à esquerda
                atual = atual.esquerda
            else:
                atual = atual.direita
        return candidato

    def antecessor(self, valor):
        """Maior valor da árvore que é MENOR que `valor` (ou None)."""
        candidato = None
        atual = self.raiz
        while atual is not None:
            if valor > atual.valor:
                candidato = atual.valor
                atual = atual.direita
            else:
                atual = atual.esquerda
        return candidato

    # ==================================================================
    # Remoção
    # ==================================================================
    def remover(self, valor):
        """Remove `valor`. Devolve True se existia."""
        if not self.contem(valor):
            return False
        self.raiz = self._remover(self.raiz, valor)
        self._tamanho -= 1
        return True

    def _remover(self, no, valor):
        """Remove `valor` da subárvore `no` e devolve a nova raiz dessa subárvore."""
        if no is None:
            return None
        if valor < no.valor:
            no.esquerda = self._remover(no.esquerda, valor)
        elif valor > no.valor:
            no.direita = self._remover(no.direita, valor)
        else:
            # Casos 1 e 2: zero ou um filho -> o filho (ou None) ocupa o lugar
            if no.esquerda is None:
                return no.direita
            if no.direita is None:
                return no.esquerda
            # Caso 3: dois filhos -> copia o sucessor e remove o sucessor
            sucessor = self._minimo_no(no.direita)
            no.valor = sucessor.valor
            no.direita = self._remover(no.direita, sucessor.valor)
        return no

    # ==================================================================
    # Percursos
    # ==================================================================
    def pre_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                resultado.append(no.valor)
                visitar(no.esquerda)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def em_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                visitar(no.esquerda)
                resultado.append(no.valor)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def pos_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                visitar(no.esquerda)
                visitar(no.direita)
                resultado.append(no.valor)

        visitar(self.raiz)
        return resultado

    def em_nivel(self):
        resultado = []
        fila = deque([self.raiz] if self.raiz else [])
        while fila:
            no = fila.popleft()
            resultado.append(no.valor)
            if no.esquerda:
                fila.append(no.esquerda)
            if no.direita:
                fila.append(no.direita)
        return resultado

    # ==================================================================
    # Outras operações clássicas
    # ==================================================================
    def altura(self):
        def calcular(no):
            if no is None:
                return -1
            return 1 + max(calcular(no.esquerda), calcular(no.direita))

        return calcular(self.raiz)

    def contar_folhas(self):
        def contar(no):
            if no is None:
                return 0
            if no.esquerda is None and no.direita is None:
                return 1
            return contar(no.esquerda) + contar(no.direita)

        return contar(self.raiz)

    def nivel(self, valor):
        """Profundidade do valor (raiz = 0) ou -1. Anda só por um caminho."""
        atual, profundidade = self.raiz, 0
        while atual is not None:
            if valor == atual.valor:
                return profundidade
            atual = atual.esquerda if valor < atual.valor else atual.direita
            profundidade += 1
        return -1

    def k_esimo_menor(self, k):
        """k-ésimo menor valor (k começa em 1): em ordem iterativo, para no k-ésimo."""
        if not 1 <= k <= self._tamanho:
            raise IndexError("k fora do intervalo")
        pilha, atual = [], self.raiz
        while True:
            while atual is not None:
                pilha.append(atual)
                atual = atual.esquerda
            atual = pilha.pop()
            k -= 1
            if k == 0:
                return atual.valor
            atual = atual.direita

    def intervalo(self, minimo, maximo):
        """Valores em [minimo, maximo], ordenados. Poda os galhos inúteis."""
        resultado = []

        def visitar(no):
            if no is None:
                return
            if minimo < no.valor:           # pode haver valores válidos à esquerda
                visitar(no.esquerda)
            if minimo <= no.valor <= maximo:
                resultado.append(no.valor)
            if no.valor < maximo:           # pode haver valores válidos à direita
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def ancestral_comum(self, a, b):
        """Ancestral comum mais próximo (LCA) de dois valores que existem na árvore.

        Desce enquanto os dois estiverem do MESMO lado; onde eles se separam
        (ou um deles é o próprio nó) está o ancestral comum.
        """
        if not (self.contem(a) and self.contem(b)):
            raise ValueError("Os dois valores precisam estar na árvore")
        atual = self.raiz
        while True:
            if a < atual.valor and b < atual.valor:
                atual = atual.esquerda
            elif a > atual.valor and b > atual.valor:
                atual = atual.direita
            else:
                return atual.valor

    def eh_valida(self):
        """Confere a regra da ABB usando limites (não basta olhar só os filhos!)."""
        def verificar(no, menor_permitido, maior_permitido):
            if no is None:
                return True
            if menor_permitido is not None and no.valor <= menor_permitido:
                return False
            if maior_permitido is not None and no.valor >= maior_permitido:
                return False
            return (verificar(no.esquerda, menor_permitido, no.valor)
                    and verificar(no.direita, no.valor, maior_permitido))

        return verificar(self.raiz, None, None)

    def __len__(self):
        return self._tamanho

    def __contains__(self, valor):
        return self.contem(valor)

    def __iter__(self):
        return iter(self.em_ordem())

    def __str__(self):
        if self.raiz is None:
            return "(árvore vazia)"
        linhas = []

        def desenhar(no, nivel):
            if no:
                desenhar(no.direita, nivel + 1)
                linhas.append("    " * nivel + str(no.valor))
                desenhar(no.esquerda, nivel + 1)

        desenhar(self.raiz, 0)
        return "\n".join(linhas)


if __name__ == "__main__":
    abb = ArvoreBinariaBusca([50, 30, 70, 20, 40, 60, 80, 35, 45])
    print(abb)
    print("Em ordem (ordenado):", abb.em_ordem())
    print("Pré-ordem:", abb.pre_ordem())
    print("Pós-ordem:", abb.pos_ordem())
    print("Em nível:", abb.em_nivel())
    print("Mínimo:", abb.minimo(), "| máximo:", abb.maximo(), "| altura:", abb.altura())
    print("Contém 60?", 60 in abb, "| contém 65?", 65 in abb)
    print("Sucessor de 45:", abb.sucessor(45), "| antecessor de 50:", abb.antecessor(50))
    print("3º menor:", abb.k_esimo_menor(3), "| valores em [33, 65]:", abb.intervalo(33, 65))
    print("Ancestral comum de 35 e 45:", abb.ancestral_comum(35, 45))

    abb.remover(20)  # caso 1: folha
    abb.remover(40)  # caso 3: dois filhos (35 e 45)
    abb.remover(30)  # caso 2: um filho
    print("\nDepois de remover 20, 40 e 30:")
    print(abb)
    print("Ainda é ABB válida?", abb.eh_valida())

    torta = ArvoreBinariaBusca(range(1, 8))
    print("\nInserindo 1..7 em ordem, a altura fica", torta.altura(), "(virou uma lista!)")
