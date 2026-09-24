r"""
ÁRVORE BINÁRIA (genérica, sem ordem entre os valores)
=====================================================

- Cada nó tem no máximo DOIS filhos: `esquerda` e `direita`.
- Raiz: o nó do topo. Folha: nó sem filhos. Nível/profundidade: distância até a raiz.

              1          <- raiz (nível 0)
            /   \
           2     3       <- nível 1
          / \     \
         4   5     7     <- nível 2 (4, 5 e 7 são folhas)

Percursos (caem em TODA prova)
------------------------------
- Pré-ordem  (Raiz, Esq, Dir): 1 2 4 5 3 7
- Em ordem   (Esq, Raiz, Dir): 4 2 5 1 3 7
- Pós-ordem  (Esq, Dir, Raiz): 4 5 2 7 3 1
- Em nível   (largura / BFS):  1 2 3 4 5 7   <- usa FILA

Definições usadas aqui (os livros variam!)
------------------------------------------
- Altura: nº de ARESTAS da raiz até a folha mais funda. Folha = 0, vazia = -1.
  (Alguns livros contam NÓS: folha = 1, vazia = 0.)
- Estritamente binária: todo nó tem 0 ou 2 filhos.
- Completa: todos os níveis cheios, exceto talvez o último, que é preenchido
  da esquerda para a direita.
- Cheia (perfeita): todos os níveis completamente cheios -> n = 2^(h+1) - 1.

Complexidade: percursos, altura, contagens e busca são O(n).
"""

from collections import deque  # fila FIFO: append = enfileirar, popleft = desenfileirar


class No:
    def __init__(self, valor, esquerda=None, direita=None):
        self.valor = valor
        self.esquerda = esquerda
        self.direita = direita


class ArvoreBinaria:
    def __init__(self, raiz=None):
        self.raiz = raiz

    # ==================================================================
    # Construção
    # ==================================================================
    @classmethod
    def de_lista_por_nivel(cls, valores):
        """Monta a árvore a partir da representação em VETOR.

        O filho esquerdo do índice i fica em 2i+1 e o direito em 2i+2.
        `None` marca uma posição sem nó.
        Ex.: [1, 2, 3, 4, 5, None, 7] gera a árvore do topo deste arquivo.
        """
        def construir(i):
            if i >= len(valores) or valores[i] is None:
                return None
            return No(valores[i], construir(2 * i + 1), construir(2 * i + 2))

        return cls(construir(0))

    @classmethod
    def de_percursos(cls, pre_ordem, em_ordem):
        """Reconstrói a árvore a partir da pré-ordem + em ordem (valores únicos).

        - O 1º da pré-ordem é a raiz.
        - Na em ordem, tudo à esquerda da raiz é a subárvore esquerda e tudo à
          direita é a subárvore direita. Repete recursivamente.
        """
        if sorted(pre_ordem) != sorted(em_ordem) or len(set(em_ordem)) != len(em_ordem):
            raise ValueError("Percursos incompatíveis ou com valores repetidos")
        posicao = {valor: i for i, valor in enumerate(em_ordem)}
        proximo = iter(pre_ordem)

        def construir(ini, fim):  # intervalo [ini, fim] da em ordem
            if ini > fim:
                return None
            valor = next(proximo)
            meio = posicao[valor]
            no = No(valor)
            no.esquerda = construir(ini, meio - 1)
            no.direita = construir(meio + 1, fim)
            return no

        return cls(construir(0, len(em_ordem) - 1))

    def inserir_por_nivel(self, valor):
        """Insere na primeira vaga encontrada no percurso em nível.

        Se a árvore era completa, continua completa.
        """
        novo = No(valor)
        if self.raiz is None:
            self.raiz = novo
            return
        fila = deque([self.raiz])
        while fila:
            no = fila.popleft()
            if no.esquerda is None:
                no.esquerda = novo
                return
            if no.direita is None:
                no.direita = novo
                return
            fila.append(no.esquerda)
            fila.append(no.direita)

    # ==================================================================
    # Percursos recursivos
    # ==================================================================
    def pre_ordem(self):
        resultado = []

        def visitar(no):
            if no is None:
                return
            resultado.append(no.valor)  # R
            visitar(no.esquerda)        # E
            visitar(no.direita)         # D

        visitar(self.raiz)
        return resultado

    def em_ordem(self):
        resultado = []

        def visitar(no):
            if no is None:
                return
            visitar(no.esquerda)        # E
            resultado.append(no.valor)  # R
            visitar(no.direita)         # D

        visitar(self.raiz)
        return resultado

    def pos_ordem(self):
        resultado = []

        def visitar(no):
            if no is None:
                return
            visitar(no.esquerda)        # E
            visitar(no.direita)         # D
            resultado.append(no.valor)  # R

        visitar(self.raiz)
        return resultado

    # ==================================================================
    # Percursos iterativos (a recursão vira uma PILHA explícita)
    # A list do Python funciona como pilha: append = empilhar, pop = desempilhar.
    # ==================================================================
    def pre_ordem_iterativa(self):
        if self.raiz is None:
            return []
        resultado = []
        pilha = [self.raiz]
        while pilha:
            no = pilha.pop()
            resultado.append(no.valor)
            # empilha a DIREITA primeiro para a ESQUERDA sair antes
            if no.direita:
                pilha.append(no.direita)
            if no.esquerda:
                pilha.append(no.esquerda)
        return resultado

    def em_ordem_iterativa(self):
        resultado = []
        pilha = []
        atual = self.raiz
        while pilha or atual is not None:
            while atual is not None:      # desce o máximo possível à esquerda
                pilha.append(atual)
                atual = atual.esquerda
            atual = pilha.pop()           # visita
            resultado.append(atual.valor)
            atual = atual.direita         # e vai para a direita
        return resultado

    def pos_ordem_iterativa(self):
        """Faz Raiz-Dir-Esq com pilha e inverte no final: vira Esq-Dir-Raiz."""
        if self.raiz is None:
            return []
        resultado = []
        pilha = [self.raiz]
        while pilha:
            no = pilha.pop()
            resultado.append(no.valor)
            if no.esquerda:
                pilha.append(no.esquerda)
            if no.direita:
                pilha.append(no.direita)
        resultado.reverse()
        return resultado

    def em_nivel(self):
        """Percurso em largura (BFS) usando FILA."""
        if self.raiz is None:
            return []
        resultado = []
        fila = deque([self.raiz])
        while fila:
            no = fila.popleft()
            resultado.append(no.valor)
            if no.esquerda:
                fila.append(no.esquerda)
            if no.direita:
                fila.append(no.direita)
        return resultado

    def por_niveis(self):
        """Igual ao em_nivel, mas separando: [[1], [2, 3], [4, 5, 7]]."""
        if self.raiz is None:
            return []
        niveis = []
        fila = deque([self.raiz])
        while fila:
            nivel_atual = []
            for _ in range(len(fila)):  # só os nós que já estavam na fila
                no = fila.popleft()
                nivel_atual.append(no.valor)
                if no.esquerda:
                    fila.append(no.esquerda)
                if no.direita:
                    fila.append(no.direita)
            niveis.append(nivel_atual)
        return niveis

    # ==================================================================
    # Medidas
    # ==================================================================
    def tamanho(self):
        def contar(no):
            if no is None:
                return 0
            return 1 + contar(no.esquerda) + contar(no.direita)

        return contar(self.raiz)

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

    def contar_nos_internos(self):
        return self.tamanho() - self.contar_folhas()

    def nivel(self, valor):
        """Profundidade do nó com `valor` (raiz = 0) ou -1 se não existir."""
        def procurar(no, profundidade):
            if no is None:
                return -1
            if no.valor == valor:
                return profundidade
            achou = procurar(no.esquerda, profundidade + 1)
            if achou != -1:
                return achou
            return procurar(no.direita, profundidade + 1)

        return procurar(self.raiz, 0)

    # ==================================================================
    # Busca e modificação
    # ==================================================================
    def buscar(self, valor):
        """Devolve o nó com `valor` ou None. Precisa olhar a árvore toda: O(n)."""
        def procurar(no):
            if no is None or no.valor == valor:
                return no
            return procurar(no.esquerda) or procurar(no.direita)

        return procurar(self.raiz)

    def contem(self, valor):
        return self.buscar(valor) is not None

    def remover(self, valor):
        """Remove a primeira ocorrência (em nível) de `valor`.

        Árvore genérica não tem ordem, então: copia o valor do nó MAIS FUNDO e
        MAIS À DIREITA para o nó a remover e apaga esse nó mais fundo.
        """
        if self.raiz is None:
            return False
        alvo = None
        ultimo = pai_do_ultimo = None
        fila = deque([(self.raiz, None)])
        while fila:
            no, pai = fila.popleft()
            if alvo is None and no.valor == valor:
                alvo = no
            ultimo, pai_do_ultimo = no, pai
            if no.esquerda:
                fila.append((no.esquerda, no))
            if no.direita:
                fila.append((no.direita, no))

        if alvo is None:
            return False
        alvo.valor = ultimo.valor
        if pai_do_ultimo is None:  # a árvore só tinha a raiz
            self.raiz = None
        elif pai_do_ultimo.direita is ultimo:
            pai_do_ultimo.direita = None
        else:
            pai_do_ultimo.esquerda = None
        return True

    def espelhar(self):
        """Troca esquerda e direita em todos os nós (imagem no espelho)."""
        def trocar(no):
            if no is None:
                return
            no.esquerda, no.direita = no.direita, no.esquerda
            trocar(no.esquerda)
            trocar(no.direita)

        trocar(self.raiz)

    # ==================================================================
    # Classificações
    # ==================================================================
    def eh_estritamente_binaria(self):
        def verificar(no):
            if no is None:
                return True
            if (no.esquerda is None) != (no.direita is None):  # só um filho
                return False
            return verificar(no.esquerda) and verificar(no.direita)

        return verificar(self.raiz)

    def eh_completa(self):
        """Em nível: depois do primeiro "buraco" não pode aparecer mais nenhum nó."""
        if self.raiz is None:
            return True
        fila = deque([self.raiz])
        viu_buraco = False
        while fila:
            no = fila.popleft()
            for filho in (no.esquerda, no.direita):
                if filho is None:
                    viu_buraco = True
                elif viu_buraco:
                    return False
                else:
                    fila.append(filho)
        return True

    def eh_cheia(self):
        return self.tamanho() == 2 ** (self.altura() + 1) - 1

    def igual_a(self, outra):
        """Mesma forma e mesmos valores."""
        def comparar(a, b):
            if a is None or b is None:
                return a is b
            return a.valor == b.valor and comparar(a.esquerda, b.esquerda) and comparar(a.direita, b.direita)

        return comparar(self.raiz, outra.raiz)

    # ==================================================================
    # Exibição
    # ==================================================================
    def __len__(self):
        return self.tamanho()

    def __contains__(self, valor):
        return self.contem(valor)

    def __str__(self):
        """Desenha a árvore DEITADA (incline a cabeça para a esquerda):
        a direita fica em cima e a esquerda embaixo."""
        if self.raiz is None:
            return "(árvore vazia)"
        linhas = []

        def desenhar(no, nivel):
            if no is None:
                return
            desenhar(no.direita, nivel + 1)
            linhas.append("    " * nivel + str(no.valor))
            desenhar(no.esquerda, nivel + 1)

        desenhar(self.raiz, 0)
        return "\n".join(linhas)


if __name__ == "__main__":
    arvore = ArvoreBinaria.de_lista_por_nivel([1, 2, 3, 4, 5, None, 7])
    print(arvore)
    print("Pré-ordem:", arvore.pre_ordem(), "| iterativa:", arvore.pre_ordem_iterativa())
    print("Em ordem: ", arvore.em_ordem(), "| iterativa:", arvore.em_ordem_iterativa())
    print("Pós-ordem:", arvore.pos_ordem(), "| iterativa:", arvore.pos_ordem_iterativa())
    print("Em nível: ", arvore.em_nivel(), "| por níveis:", arvore.por_niveis())
    print("Tamanho:", arvore.tamanho(), "| altura:", arvore.altura(),
          "| folhas:", arvore.contar_folhas(), "| internos:", arvore.contar_nos_internos())
    print("Nível do 5:", arvore.nivel(5))
    print("Estritamente binária?", arvore.eh_estritamente_binaria(),
          "| completa?", arvore.eh_completa(), "| cheia?", arvore.eh_cheia())

    copia = ArvoreBinaria.de_percursos(arvore.pre_ordem(), arvore.em_ordem())
    print("Reconstruída a partir dos percursos é igual?", copia.igual_a(arvore))

    arvore.espelhar()
    print("Espelhada (em ordem):", arvore.em_ordem())
