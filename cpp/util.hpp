// Funções de exibição usadas pelas demonstrações (main.cpp de cada pasta).
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Junta os elementos com um separador: juntar({1, 2, 3}, " -> ") == "1 -> 2 -> 3"
template <typename T>
std::string juntar(const std::vector<T>& valores, const std::string& separador = ", ") {
    std::ostringstream saida;
    for (std::size_t i = 0; i < valores.size(); i++) {
        if (i > 0) saida << separador;
        saida << valores[i];
    }
    return saida.str();
}

// Formato de lista: [1, 2, 3]
template <typename T>
std::string em_colchetes(const std::vector<T>& valores) {
    return "[" + juntar(valores) + "]";
}

inline void titulo(const std::string& texto) {
    std::cout << "\n== " << texto << " ==\n";
}
