// LISTA HETEROGÊNEA
//
// - Cada nó pode guardar um TIPO DIFERENTE de dado (inteiro, real, caractere, texto).
// - Igual ao C: um `enum` (a etiqueta do tipo) + uma `union` (o valor). A union
//   reserva espaço para o MAIOR dos campos, e só um deles é válido por vez.
//
//     [inteiro: 10] -> [real: 3.14] -> [caractere: 'a'] -> [texto: "oi"] -> nullptr
//
// Complexidade: igual à lista encadeada (inserção no fim O(1) com ponteiro `fim`).
//
// Pega-ratão
// - Ler o campo errado da union devolve lixo. Por isso sempre olhamos a
//   etiqueta `tipo` antes de usar o valor.
// - O texto fica num `char*` alocado com new[]; ao remover o nó é preciso
//   liberar esse vetor também.
// - Em C++, `bool` converte sozinho para `int` e um literal "oi" (const char*)
//   converte sozinho para `bool`! Por isso existe uma sobrecarga para const char*
//   e a de bool é apagada (`= delete`): `inserir(true)` nem compila.
#pragma once

#include <array>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

enum class Tipo { INTEIRO, REAL, CARACTERE, TEXTO };

inline const char* nome_do_tipo(Tipo tipo) {
    switch (tipo) {
        case Tipo::INTEIRO: return "inteiro";
        case Tipo::REAL: return "real";
        case Tipo::CARACTERE: return "caractere";
        case Tipo::TEXTO: return "texto";
    }
    return "?";
}

class ListaHeterogenea {
    struct No {
        Tipo tipo;  // a "etiqueta"
        union {     // o dado: só o campo indicado por `tipo` é válido
            int inteiro;
            double real;
            char caractere;
            char* texto;
        };
        No* proximo = nullptr;
    };

public:
    ListaHeterogenea() = default;
    ~ListaHeterogenea() { limpar(); }
    ListaHeterogenea(const ListaHeterogenea&) = delete;
    ListaHeterogenea& operator=(const ListaHeterogenea&) = delete;

    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }

    // ---------------------------------------------------------------- inserção
    // Sempre no fim, para manter a ordem de chegada. O compilador escolhe a
    // sobrecarga certa pelo tipo do argumento.
    void inserir(int valor) {
        No* no = novo_no(Tipo::INTEIRO);
        no->inteiro = valor;
    }

    void inserir(double valor) {
        No* no = novo_no(Tipo::REAL);
        no->real = valor;
    }

    void inserir(char valor) {
        No* no = novo_no(Tipo::CARACTERE);
        no->caractere = valor;
    }

    void inserir(const char* valor) {
        No* no = novo_no(Tipo::TEXTO);
        no->texto = new char[std::strlen(valor) + 1];
        std::strcpy(no->texto, valor);
    }

    void inserir(const std::string& valor) { inserir(valor.c_str()); }

    void inserir(bool) = delete;  // veja o pega-ratão no topo do arquivo

    // ---------------------------------------------------------------- remoção
    // Remove o primeiro nó e devolve {tipo, valor em texto}.
    std::pair<Tipo, std::string> remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        No* no = inicio_;
        std::pair<Tipo, std::string> removido{no->tipo, valor_em_texto(no)};
        inicio_ = no->proximo;
        if (inicio_ == nullptr) fim_ = nullptr;  // esvaziou: o fim também precisa ser zerado
        if (no->tipo == Tipo::TEXTO) delete[] no->texto;
        delete no;
        tamanho_--;
        return removido;
    }

    // ---------------------------------------------------------------- consultas
    // Quantidade de nós de cada tipo, na ordem do enum.
    std::array<int, 4> contar_por_tipo() const {
        std::array<int, 4> contagem{};
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo)
            contagem[static_cast<int>(atual->tipo)]++;
        return contagem;
    }

    // Os valores de um determinado tipo, em texto, na ordem da lista.
    std::vector<std::string> filtrar(Tipo tipo) const {
        std::vector<std::string> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo)
            if (atual->tipo == tipo) resultado.push_back(valor_em_texto(atual));
        return resultado;
    }

    // Soma inteiros e reais, ignorando caracteres e textos.
    double somar_numeros() const {
        double soma = 0;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) {
            if (atual->tipo == Tipo::INTEIRO) soma += atual->inteiro;
            else if (atual->tipo == Tipo::REAL) soma += atual->real;
        }
        return soma;
    }

    std::vector<Tipo> tipos() const {
        std::vector<Tipo> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) resultado.push_back(atual->tipo);
        return resultado;
    }

    std::string para_texto() const {
        if (vazia()) return "nullptr";
        std::string saida;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) {
            std::string valor = valor_em_texto(atual);
            if (atual->tipo == Tipo::CARACTERE) valor = "'" + valor + "'";
            if (atual->tipo == Tipo::TEXTO) valor = "\"" + valor + "\"";
            saida += "[" + std::string(nome_do_tipo(atual->tipo)) + ": " + valor + "] -> ";
        }
        return saida + "nullptr";
    }

    void limpar() {
        while (!vazia()) remover_inicio();
    }

private:
    No* inicio_ = nullptr;
    No* fim_ = nullptr;
    int tamanho_ = 0;

    // Cria o nó, liga no fim e devolve para o chamador preencher a union.
    No* novo_no(Tipo tipo) {
        No* novo = new No;
        novo->tipo = tipo;
        if (vazia())
            inicio_ = novo;
        else
            fim_->proximo = novo;
        fim_ = novo;
        tamanho_++;
        return novo;
    }

    // Olha a etiqueta ANTES de ler a union.
    static std::string valor_em_texto(const No* no) {
        std::ostringstream saida;
        switch (no->tipo) {
            case Tipo::INTEIRO: saida << no->inteiro; break;
            case Tipo::REAL: saida << no->real; break;
            case Tipo::CARACTERE: saida << no->caractere; break;
            case Tipo::TEXTO: saida << no->texto; break;
        }
        return saida.str();
    }
};
