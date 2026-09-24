#include <iostream>
#include <string>

#include "../util.hpp"
#include "deque.hpp"
#include "fila_duas_pilhas.hpp"
#include "fila_encadeada.hpp"
#include "fila_prioridade.hpp"
#include "fila_sequencial.hpp"

template <typename Fila>
std::string fila_em_texto(const Fila& fila) {
    return "início -> " + em_colchetes(fila.para_vetor()) + " <- fim";
}

int main() {
    titulo("Fila sequencial circular");
    FilaSequencial<int> fila(4);
    for (int valor : {10, 20, 30, 40}) fila.enfileirar(valor);
    std::cout << "Fila: " << fila_em_texto(fila) << " | cheia? " << (fila.cheia() ? "sim" : "não") << "\n";
    int primeiro = fila.desenfileirar();
    int segundo = fila.desenfileirar();
    std::cout << "Saíram: " << primeiro << " e " << segundo << "\n";
    fila.enfileirar(50);
    fila.enfileirar(60);  // estas entram no começo do vetor (deu a volta)
    std::cout << "Fila: " << fila_em_texto(fila) << " | início = " << fila.indice_inicio()
              << " | fim = " << fila.indice_fim() << "\n";
    std::cout << "Frente: " << fila.frente() << " | último: " << fila.ultimo() << "\n";

    titulo("Fila encadeada");
    FilaEncadeada<std::string> banco;
    for (const char* pessoa : {"Ana", "Bia", "Caio"}) banco.enfileirar(pessoa);
    std::cout << "Fila: " << fila_em_texto(banco) << "\n";
    std::string atendida = banco.desenfileirar();
    std::cout << "Atendida: " << atendida << " | fila: " << fila_em_texto(banco) << "\n";
    banco.enfileirar("Davi");
    std::cout << "Chegou Davi: " << fila_em_texto(banco) << " | frente: " << banco.frente()
              << " | último: " << banco.ultimo() << "\n";

    titulo("Deque");
    Deque<int> deque;
    deque.inserir_fim(20);
    deque.inserir_fim(30);
    deque.inserir_inicio(10);
    deque.inserir_inicio(5);
    std::cout << "Deque: " << em_colchetes(deque.para_vetor()) << "\n";
    int do_inicio = deque.remover_inicio();
    int do_fim = deque.remover_fim();
    std::cout << "Removido do início: " << do_inicio << " | removido do fim: " << do_fim << "\n";
    std::cout << "Deque: " << em_colchetes(deque.para_vetor()) << " | primeiro: " << deque.primeiro()
              << " | último: " << deque.ultimo() << "\n";

    titulo("Fila de prioridade");
    FilaPrioridade<std::string> pronto_socorro;
    pronto_socorro.enfileirar("corte leve", 4);
    pronto_socorro.enfileirar("parada cardíaca", 1);
    pronto_socorro.enfileirar("febre", 3);
    pronto_socorro.enfileirar("fratura", 2);
    pronto_socorro.enfileirar("dor de cabeça", 3);
    std::cout << "Fila:";
    for (const auto& [valor, prioridade] : pronto_socorro.em_ordem())
        std::cout << " " << valor << "(p=" << prioridade << ")";
    std::cout << "\n";
    while (!pronto_socorro.vazia()) std::cout << "Atendendo: " << pronto_socorro.desenfileirar() << "\n";

    titulo("Fila com duas pilhas");
    FilaComDuasPilhas<int> duas_pilhas;
    for (int valor : {1, 2, 3}) duas_pilhas.enfileirar(valor);
    std::cout << "Saiu: " << duas_pilhas.desenfileirar() << "\n";
    duas_pilhas.enfileirar(4);
    std::cout << "Frente: " << duas_pilhas.frente() << "\n";
    while (!duas_pilhas.vazia()) std::cout << "Saiu: " << duas_pilhas.desenfileirar() << "\n";
}
