"""
FILA COM DUAS PILHAS (exercício clássico)
=========================================

Como fazer uma fila (FIFO) usando só pilhas (LIFO)?

- Pilha de ENTRADA: todo `enfileirar` empilha aqui.
- Pilha de SAÍDA: todo `desenfileirar` desempilha daqui.
- Se a saída estiver vazia, despeja TODA a entrada nela. Ao passar de uma pilha
  para a outra a ordem inverte, e o mais antigo fica no topo.

  enfileirar 1, 2, 3:   entrada = [1, 2, 3]   saída = []
  desenfileirar:        entrada = []          saída = [3, 2, 1] -> sai o 1

Complexidade: enfileirar O(1); desenfileirar O(1) amortizado (cada elemento
é transferido no máximo uma vez).

Rode a partir da pasta `python/`:  python3 -m filas.fila_duas_pilhas
"""

from pilhas.pilha_encadeada import PilhaEncadeada


class FilaComDuasPilhas:
    def __init__(self):
        self._entrada = PilhaEncadeada()
        self._saida = PilhaEncadeada()

    def vazia(self):
        return self._entrada.vazia() and self._saida.vazia()

    def tamanho(self):
        return len(self._entrada) + len(self._saida)

    def enfileirar(self, valor):
        self._entrada.empilhar(valor)

    def _transferir_se_preciso(self):
        # Só transfere quando a saída acaba. Transferir antes disso
        # colocaria elementos novos na frente dos antigos.
        if self._saida.vazia():
            while not self._entrada.vazia():
                self._saida.empilhar(self._entrada.desempilhar())

    def desenfileirar(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        self._transferir_se_preciso()
        return self._saida.desempilhar()

    def frente(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        self._transferir_se_preciso()
        return self._saida.topo()

    def __len__(self):
        return self.tamanho()


if __name__ == "__main__":
    fila = FilaComDuasPilhas()
    for valor in (1, 2, 3):
        fila.enfileirar(valor)
    print("Saiu:", fila.desenfileirar())
    fila.enfileirar(4)
    print("Frente:", fila.frente())
    while not fila.vazia():
        print("Saiu:", fila.desenfileirar())
