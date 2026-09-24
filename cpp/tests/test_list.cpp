// Testes da lista sequencial.
// A ideia central: fazer milhares de operações ALEATÓRIAS na nossa lista e num
// std::vector ao mesmo tempo, conferindo a cada passo que os dois continuam iguais.
#include <algorithm>
#include <random>

#include "../list/lista_sequencial.hpp"
#include "teste.hpp"

TESTE(comeca_vazia) {
    ListaSequencial<int> lista(5);
    VERIFICAR(lista.vazia());
    VERIFICAR_IGUAL(lista.tamanho(), 0);
    VERIFICAR_LANCA(lista.remover_inicio(), std::out_of_range);
    VERIFICAR_LANCA(lista.remover_fim(), std::out_of_range);
}

TESTE(capacidade_invalida) { VERIFICAR_LANCA(ListaSequencial<int>(0), std::invalid_argument); }

TESTE(capacidade_cheia) {
    ListaSequencial<int> lista(3);
    for (int valor : {1, 2, 3}) lista.inserir_fim(valor);
    VERIFICAR(lista.cheia());
    VERIFICAR_LANCA(lista.inserir_fim(4), std::overflow_error);
    VERIFICAR_LANCA(lista.inserir_inicio(0), std::overflow_error);
}

TESTE(posicoes_invalidas) {
    ListaSequencial<int> lista(5);
    lista.inserir_fim(10);
    VERIFICAR_LANCA(lista.obter(1), std::out_of_range);
    VERIFICAR_LANCA(lista.obter(-1), std::out_of_range);
    VERIFICAR_LANCA(lista.inserir_posicao(3, 99), std::out_of_range);
    VERIFICAR_LANCA(lista.remover_posicao(1), std::out_of_range);
}

TESTE(insercoes_buscas_e_inversao) {
    ListaSequencial<int> lista(10);
    lista.inserir_fim(20);
    lista.inserir_inicio(10);
    lista.inserir_posicao(2, 30);
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{10, 20, 30}));
    VERIFICAR_IGUAL(lista.buscar(30), 2);
    VERIFICAR_IGUAL(lista.buscar(99), -1);
    VERIFICAR(lista.remover_valor(20));
    VERIFICAR(!lista.remover_valor(20));
    lista.inverter();
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{30, 10}));
    lista.limpar();
    VERIFICAR(lista.vazia());
}

TESTE(operacoes_aleatorias_contra_std_vector) {
    std::mt19937 sorteio(1);
    ListaSequencial<int> lista(60);
    std::vector<int> modelo;
    for (int passo = 0; passo < 5000; passo++) {
        int operacao = sorteio() % 6;
        int valor = sorteio() % 30;
        int n = static_cast<int>(modelo.size());
        if (operacao == 0 && n < 60) {
            int pos = sorteio() % (n + 1);
            lista.inserir_posicao(pos, valor);
            modelo.insert(modelo.begin() + pos, valor);
        } else if (operacao == 1 && n > 0) {
            int pos = sorteio() % n;
            VERIFICAR_IGUAL(lista.remover_posicao(pos), modelo[pos]);
            modelo.erase(modelo.begin() + pos);
        } else if (operacao == 2) {
            auto achou = std::find(modelo.begin(), modelo.end(), valor);
            VERIFICAR_IGUAL(lista.remover_valor(valor), achou != modelo.end());
            if (achou != modelo.end()) modelo.erase(achou);
        } else if (operacao == 3 && n > 0) {
            int pos = sorteio() % n;
            lista.alterar(pos, valor);
            modelo[pos] = valor;
        } else if (operacao == 4) {
            lista.inverter();
            std::reverse(modelo.begin(), modelo.end());
        } else if (operacao == 5) {
            auto achou = std::find(modelo.begin(), modelo.end(), valor);
            VERIFICAR_IGUAL(lista.buscar(valor), achou == modelo.end() ? -1 : int(achou - modelo.begin()));
        }
        VERIFICAR_IGUAL(lista.para_vetor(), modelo);
    }
}

int main() { return rodar_testes(); }
