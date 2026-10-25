/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 *
 * Pilha estatica: TAD usado para operadores e para valores no calculo da RPN.
 */

#ifndef PILHA_ESTATICA_HPP
#define PILHA_ESTATICA_HPP

#include <iostream>

#ifndef TAM
#define TAM 50
#endif // TAM

template <typename T>
struct PilhaEstatica{
    T vetor[TAM];
    int topo;
};

template <typename T>
void inicializar(PilhaEstatica<T> &p){
    p.topo = -1;
}

template <typename T>
bool Vazia(const PilhaEstatica<T> &p){
    return p.topo == -1;
}

template <typename T>
bool Cheia(const PilhaEstatica<T> &p){
    return p.topo == TAM - 1;
}

template <typename T>
bool push(PilhaEstatica<T> &p, T valor){
    if( Cheia(p) ) return false;
    p.vetor[++p.topo] = valor;
    return true;
}

template <typename T>
bool pop(PilhaEstatica<T> &p, T &valor){
    if( Vazia(p) ) return false;
    valor = p.vetor[p.topo--];
    return true;
}

template <typename T>
bool top(const PilhaEstatica<T> &p, T &valor){
    if( Vazia(p) ) return false;
    valor = p.vetor[p.topo];
    return true;
}

// Percorre a pilha do topo para a base sem alterá-la, só mostrando os elementos.
template <typename T>
void mostrar(const PilhaEstatica<T> &p){
    for( int i = p.topo; i >= 0; i-- )
        std::cout << p.vetor[i] << " ";
}

// Não há memória dinâmica a desalocar: basta marcar a pilha como vazia.
template <typename T>
void liberar(PilhaEstatica<T> &p){
    inicializar(p);
}

#endif // PILHA_ESTATICA_HPP






