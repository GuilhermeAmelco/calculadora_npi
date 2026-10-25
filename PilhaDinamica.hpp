/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 *
 * Pilha dinamica encadeada: TAD usado para operadores e valores.
 */

#ifndef PILHA_DINAMICA_HPP
#define PILHA_DINAMICA_HPP

#include <iostream>
#include <new>

template <typename T>
struct NoPilha{
    T info;
    NoPilha<T> *eloA, *eloP;
};

template <typename T>
struct PilhaDinamica{
    NoPilha<T> *topo;
};

template <typename T>
void inicializar(PilhaDinamica<T> &p){
    p.topo = nullptr;
}

template <typename T>
bool Vazia(const PilhaDinamica<T> &p){
    return p.topo == nullptr;
}

// Pilha encadeada não tem capacidade fixa: só "enche" quando a alocação
// de um novo nó falha, e isso é reportado por push().
template <typename T>
bool Cheia(const PilhaDinamica<T> &){
    return false;
}

template <typename T>
bool push(PilhaDinamica<T> &p, T valor){
    NoPilha<T> *novo = new (std::nothrow) NoPilha<T>;

    if( novo == nullptr ) return false;
    novo->info = valor;
    novo->eloA = nullptr;
    novo->eloP = nullptr;

    if( p.topo == nullptr ){
        p.topo = novo;
    }
    else{
        p.topo->eloP = novo;
        novo->eloA = p.topo;
        p.topo = novo;
    }
    return true;
}

template <typename T>
bool pop(PilhaDinamica<T> &p, T &valor){
    if( Vazia(p) ) return false;

    valor = p.topo->info;
    NoPilha<T> *aux = p.topo;

    p.topo = p.topo->eloA;
    if( p.topo != nullptr ) p.topo->eloP = nullptr;

    delete aux;
    return true;
}

template <typename T>
bool top(const PilhaDinamica<T> &p, T &valor){
    if( Vazia(p) ) return false;
    valor = p.topo->info;
    return true;
}

// Percorre a pilha do topo para a base sem alterá-la, só mostrando os elementos.
template <typename T>
void mostrar(const PilhaDinamica<T> &p){
    NoPilha<T> *aux = p.topo;

    while( aux != nullptr ){
        std::cout << aux->info << " ";
        aux = aux->eloA;
    }
}

template <typename T>
void liberar(PilhaDinamica<T> &p){
    NoPilha<T> *aux = p.topo;
    while( aux != nullptr ){
        NoPilha<T> *ant = aux->eloA;
        delete aux;
        aux = ant;
    }
    p.topo = nullptr;
}

#endif // PILHA_DINAMICA_HPP






