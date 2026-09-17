"""
LISTA ENCADEADA ORDENADA
========================

- Lista encadeada que SEMPRE mantém os valores em ordem crescente.
- Não existe "inserir no início/fim": o usuário só chama `inserir(valor)` e a
  lista descobre sozinha a posição certa.
- Como está ordenada, a busca pode PARAR CEDO: se achou um valor maior do que
  o procurado, ele não está na lista.

Complexidade
------------
| operação                  | custo    |
|---------------------------|----------|
| inserir                   | O(n)     |
| remover / buscar          | O(n)     |  (mas para cedo em média)
| mínimo                    | O(1)     |
| máximo                    | O(n)     |
| mesclar duas listas       | O(n + m) |

Pega-ratão
----------
- Inserir antes do primeiro (lista vazia ou valor menor que o início) é um caso
  especial: muda a cabeça.
- Na ordenada não dá para usar busca binária: não existe acesso direto ao meio.
"""


class No:
    def __init__(self, valor, proximo=None):
        self.valor = valor
        self.proximo = proximo


class ListaOrdenada:
    def __init__(self, valores=()):
        self.inicio = None
        self._tamanho = 0
        for valor in valores:
            self.inserir(valor)

    def vazia(self):
        return self.inicio is None

    def tamanho(self):
        return self._tamanho

    def inserir(self, valor):
        """Insere mantendo a ordem. Repetidos entram depois dos iguais."""
        if self.inicio is None or valor < self.inicio.valor:
            self.inicio = No(valor, self.inicio)  # vira a nova cabeça
        else:
            atual = self.inicio
            # anda enquanto o PRÓXIMO ainda for menor ou igual ao valor
            while atual.proximo is not None and atual.proximo.valor <= valor:
                atual = atual.proximo
            atual.proximo = No(valor, atual.proximo)
        self._tamanho += 1

    def remover(self, valor):
        """Remove uma ocorrência de `valor`. Devolve True se removeu."""
        anterior = None
        atual = self.inicio
        while atual is not None and atual.valor < valor:
            anterior = atual
            atual = atual.proximo

        if atual is None or atual.valor != valor:  # passou do ponto: não existe
            return False
        if anterior is None:
            self.inicio = atual.proximo
        else:
            anterior.proximo = atual.proximo
        self._tamanho -= 1
        return True

    def remover_inicio(self):
        """Remove e devolve o MENOR valor."""
        if self.vazia():
            raise IndexError("Lista vazia")
        valor = self.inicio.valor
        self.inicio = self.inicio.proximo
        self._tamanho -= 1
        return valor

    def buscar(self, valor):
        """Devolve a posição de `valor` ou -1 (para cedo ao passar do valor)."""
        atual = self.inicio
        pos = 0
        while atual is not None and atual.valor <= valor:
            if atual.valor == valor:
                return pos
            atual = atual.proximo
            pos += 1
        return -1

    def obter(self, pos):
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        atual = self.inicio
        for _ in range(pos):
            atual = atual.proximo
        return atual.valor

    def minimo(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self.inicio.valor

    def maximo(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        atual = self.inicio
        while atual.proximo is not None:
            atual = atual.proximo
        return atual.valor

    def mesclar(self, outra):
        """Junta duas listas ordenadas em uma NOVA lista ordenada -> O(n + m).

        É o mesmo passo de "intercalação" usado no Merge Sort.
        """
        resultado = ListaOrdenada()
        a, b = self.inicio, outra.inicio
        cauda = None  # último nó do resultado, para inserir no fim em O(1)

        while a is not None or b is not None:
            if b is None or (a is not None and a.valor <= b.valor):
                valor, a = a.valor, a.proximo
            else:
                valor, b = b.valor, b.proximo

            novo = No(valor)
            if cauda is None:
                resultado.inicio = novo
            else:
                cauda.proximo = novo
            cauda = novo
            resultado._tamanho += 1
        return resultado

    def limpar(self):
        self.inicio = None
        self._tamanho = 0

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
    lista = ListaOrdenada()
    for valor in (40, 10, 30, 20, 25):
        lista.inserir(valor)
        print(f"inseriu {valor:>2}:", lista)
    print("Posição do 30:", lista.buscar(30), "| posição do 35:", lista.buscar(35))
    print("Mínimo:", lista.minimo(), "| máximo:", lista.maximo())
    lista.remover(25)
    print("Sem o 25:", lista)
    outra = ListaOrdenada([5, 15, 35, 50])
    print("Mesclando com", outra, "->", lista.mesclar(outra))
