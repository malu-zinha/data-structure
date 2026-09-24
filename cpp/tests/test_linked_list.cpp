// Testes das listas encadeadas.
// Todas as listas com a mesma interface passam pelo mesmo "contrato": milhares de
// operações aleatórias comparadas com um std::vector. Se houver qualquer erro de
// ponteiro, o teste acusa (e rodando com -fsanitize=address, vazamentos também).
#include <algorithm>
#include <random>

#include "../linked-list/lista_circular.hpp"
#include "../linked-list/lista_circular_dupla.hpp"
#include "../linked-list/lista_duplamente_encadeada.hpp"
#include "../linked-list/lista_encadeada.hpp"
#include "../linked-list/lista_heterogenea.hpp"
#include "../linked-list/lista_ordenada.hpp"
#include "teste.hpp"

template <typename Lista>
void contrato_lista(unsigned semente) {
    Lista vazia;
    VERIFICAR(vazia.vazia());
    VERIFICAR_LANCA(vazia.remover_inicio(), std::out_of_range);
    VERIFICAR_LANCA(vazia.remover_fim(), std::out_of_range);
    VERIFICAR_LANCA(vazia.obter(0), std::out_of_range);
    VERIFICAR_LANCA(vazia.inserir_posicao(1, 5), std::out_of_range);

    std::mt19937 sorteio(semente);
    Lista lista;
    std::vector<int> modelo;
    for (int passo = 0; passo < 4000; passo++) {
        int operacao = sorteio() % 9;
        int valor = sorteio() % 20;
        int n = static_cast<int>(modelo.size());
        if (operacao == 0) {
            lista.inserir_inicio(valor);
            modelo.insert(modelo.begin(), valor);
        } else if (operacao == 1) {
            lista.inserir_fim(valor);
            modelo.push_back(valor);
        } else if (operacao == 2) {
            int pos = sorteio() % (n + 1);
            lista.inserir_posicao(pos, valor);
            modelo.insert(modelo.begin() + pos, valor);
        } else if (operacao == 3 && n > 0) {
            VERIFICAR_IGUAL(lista.remover_inicio(), modelo.front());
            modelo.erase(modelo.begin());
        } else if (operacao == 4 && n > 0) {
            VERIFICAR_IGUAL(lista.remover_fim(), modelo.back());
            modelo.pop_back();
        } else if (operacao == 5 && n > 0) {
            int pos = sorteio() % n;
            VERIFICAR_IGUAL(lista.remover_posicao(pos), modelo[pos]);
            modelo.erase(modelo.begin() + pos);
        } else if (operacao == 6) {
            auto achou = std::find(modelo.begin(), modelo.end(), valor);
            VERIFICAR_IGUAL(lista.remover_valor(valor), achou != modelo.end());
            if (achou != modelo.end()) modelo.erase(achou);
        } else if (operacao == 7 && n > 0) {
            int pos = sorteio() % n;
            lista.alterar(pos, valor);
            modelo[pos] = valor;
            VERIFICAR_IGUAL(lista.obter(pos), valor);
        } else if (operacao == 8) {
            lista.inverter();
            std::reverse(modelo.begin(), modelo.end());
        }
        VERIFICAR_IGUAL(lista.tamanho(), static_cast<int>(modelo.size()));
        VERIFICAR_IGUAL(lista.para_vetor(), modelo);
        auto achou = std::find(modelo.begin(), modelo.end(), valor);
        VERIFICAR_IGUAL(lista.buscar(valor), achou == modelo.end() ? -1 : int(achou - modelo.begin()));
    }
    lista.limpar();
    VERIFICAR(lista.vazia());
}

TESTE(lista_encadeada) { contrato_lista<ListaEncadeada<int>>(1); }
TESTE(lista_duplamente_encadeada) { contrato_lista<ListaDuplamenteEncadeada<int>>(2); }
TESTE(lista_circular) { contrato_lista<ListaCircular<int>>(3); }
TESTE(lista_circular_dupla) { contrato_lista<ListaCircularDupla<int>>(4); }

TESTE(dupla_percorre_nos_dois_sentidos) {
    ListaDuplamenteEncadeada<int> lista;
    for (int valor : {1, 2, 3, 4}) lista.inserir_fim(valor);
    lista.remover_posicao(1);
    lista.inserir_posicao(2, 9);
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{1, 3, 9, 4}));
    VERIFICAR_IGUAL(lista.para_vetor_reverso(), (std::vector<int>{4, 9, 3, 1}));
}

TESTE(circular_rotacionar) {
    ListaCircular<int> lista;
    for (int valor : {1, 2, 3, 4}) lista.inserir_fim(valor);
    lista.rotacionar(1);
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{2, 3, 4, 1}));
    lista.rotacionar(7);  // 7 % 4 = 3 passos
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{1, 2, 3, 4}));
}

TESTE(josephus) {
    auto [ordem, sobrevivente] = problema_de_josephus(7, 3);
    VERIFICAR_IGUAL(ordem, (std::vector<int>{3, 6, 2, 7, 5, 1}));
    VERIFICAR_IGUAL(sobrevivente, 4);
    VERIFICAR_IGUAL(problema_de_josephus(1, 5).second, 1);
    VERIFICAR_LANCA(problema_de_josephus(0, 2), std::invalid_argument);
}

TESTE(circular_dupla_pontas_e_rotacao) {
    ListaCircularDupla<int> lista;
    for (int valor : {10, 20, 30, 40}) lista.inserir_fim(valor);
    VERIFICAR_IGUAL(lista.ultimo(), 40);
    VERIFICAR_IGUAL(lista.para_vetor_reverso(), (std::vector<int>{40, 30, 20, 10}));
    lista.rotacionar(-1);
    VERIFICAR_IGUAL(lista.para_vetor(), (std::vector<int>{40, 10, 20, 30}));
    lista.rotacionar(2);
    VERIFICAR_IGUAL(lista.primeiro(), 20);
}

TESTE(ordenada_acompanha_o_modelo) {
    std::mt19937 sorteio(5);
    ListaOrdenada<int> lista;
    std::vector<int> modelo;
    for (int passo = 0; passo < 3000; passo++) {
        int valor = sorteio() % 50;
        if (sorteio() % 3 != 0) {
            lista.inserir(valor);
            modelo.insert(std::upper_bound(modelo.begin(), modelo.end(), valor), valor);
        } else {
            auto achou = std::lower_bound(modelo.begin(), modelo.end(), valor);
            bool existe = achou != modelo.end() && *achou == valor;
            VERIFICAR_IGUAL(lista.remover(valor), existe);
            if (existe) modelo.erase(achou);
        }
        VERIFICAR_IGUAL(lista.para_vetor(), modelo);
    }
}

TESTE(ordenada_busca_minimo_maximo_e_mesclar) {
    ListaOrdenada<int> lista{40, 10, 30, 20};
    VERIFICAR_IGUAL(lista.buscar(30), 2);
    VERIFICAR_IGUAL(lista.buscar(35), -1);
    VERIFICAR_IGUAL(lista.minimo(), 10);
    VERIFICAR_IGUAL(lista.maximo(), 40);
    ListaOrdenada<int> outra{5, 15, 35, 50};
    VERIFICAR_IGUAL(lista.mesclar(outra).para_vetor(), (std::vector<int>{5, 10, 15, 20, 30, 35, 40, 50}));
    VERIFICAR_IGUAL(lista.remover_inicio(), 10);

    ListaOrdenada<int> vazia;
    VERIFICAR_LANCA(vazia.minimo(), std::out_of_range);
    VERIFICAR_LANCA(vazia.maximo(), std::out_of_range);
    VERIFICAR_IGUAL(vazia.mesclar(outra).para_vetor(), outra.para_vetor());
}

TESTE(heterogenea_guarda_o_tipo_de_cada_no) {
    ListaHeterogenea lista;
    lista.inserir(10);
    lista.inserir(2.5);
    lista.inserir('x');
    lista.inserir("texto");
    lista.inserir(std::string("outro"));
    VERIFICAR_IGUAL(lista.tipos(), (std::vector<Tipo>{Tipo::INTEIRO, Tipo::REAL, Tipo::CARACTERE, Tipo::TEXTO,
                                                      Tipo::TEXTO}));
    VERIFICAR_IGUAL(lista.contar_por_tipo(), (std::array<int, 4>{1, 1, 1, 2}));
    VERIFICAR_IGUAL(lista.filtrar(Tipo::TEXTO), (std::vector<std::string>{"texto", "outro"}));
    VERIFICAR_IGUAL(lista.somar_numeros(), 12.5);
    auto [tipo, valor] = lista.remover_inicio();
    VERIFICAR(tipo == Tipo::INTEIRO);
    VERIFICAR_IGUAL(valor, std::string("10"));
}

TESTE(heterogenea_remocao_zera_o_fim) {
    ListaHeterogenea lista;
    lista.inserir(1);
    lista.remover_inicio();
    VERIFICAR(lista.vazia());
    lista.inserir(2);  // se o fim não tivesse sido zerado, isto usaria memória liberada
    VERIFICAR_IGUAL(lista.para_texto(), std::string("[inteiro: 2] -> nullptr"));
    VERIFICAR_LANCA((lista.remover_inicio(), lista.remover_inicio()), std::out_of_range);
}

int main() { return rodar_testes(); }
