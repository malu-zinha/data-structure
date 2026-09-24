# Uso (sempre de dentro da pasta cpp/):
#   make                 compila todas as demonstrações
#   make linked-list     compila e roda a demonstração de uma pasta
#   make test            compila e roda todos os testes
#   make clean           apaga a pasta build/
#
# Os testes rodam com AddressSanitizer: acessar memória já liberada (ou fora do
# vetor) derruba o teste na hora. No Linux, vazamento de nó (new sem delete) também.

CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -O1 -g
SANITIZE := -fsanitize=address,undefined

PASTAS := list linked-list stack queue heap tree graphs
TESTES := $(patsubst tests/%.cpp,build/%,$(wildcard tests/test_*.cpp))
HEADERS := $(wildcard */*.hpp) util.hpp

.PHONY: all test clean $(PASTAS)

all: $(addprefix build/,$(PASTAS))

build/%: %/main.cpp $(HEADERS) | build
	$(CXX) $(CXXFLAGS) $< -o $@

build/test_%: tests/test_%.cpp $(HEADERS) | build
	$(CXX) $(CXXFLAGS) $(SANITIZE) $< -o $@

$(PASTAS): %: build/%
	@./build/$@

test: $(TESTES)
	@falhou=0; for teste in $(TESTES); do \
		echo "== $$teste"; ./$$teste || falhou=1; \
	done; exit $$falhou

build:
	mkdir -p build

clean:
	rm -rf build
