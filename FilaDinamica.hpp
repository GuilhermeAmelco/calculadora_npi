/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 *
 * Fila dinamica encadeada: TAD usado como entrada e como saida em RPN.
 */

#ifndef FILA_DINAMICA_HPP
#define FILA_DINAMICA_HPP

#include <iostream>
#include <new>

template <typename T>
struct NoFila
{
    T info;
    NoFila<T> *eloA, *eloP;
};

template <typename T>
struct FilaDinamica
{
    NoFila<T> *comeco;
    NoFila<T> *fim;
};

template <typename T>
void inicializar(FilaDinamica<T> &f)
{
    f.comeco = nullptr;
    f.fim = nullptr;
}

template <typename T>
bool Vazia(const FilaDinamica<T> &f)
{
    return f.comeco == nullptr;
}

// Fila encadeada não tem capacidade fixa: só "enche" quando a alocação
// de um novo nó falha, e isso é reportado por queue().
template <typename T>
bool Cheia(const FilaDinamica<T> &)
{
    return false;
}

template <typename T>
bool queue(FilaDinamica<T> &f, T valor)
{
    NoFila<T> *novo = new (std::nothrow) NoFila<T>;

    if (novo == nullptr)
        return false;
    novo->info = valor;
    novo->eloA = nullptr;
    novo->eloP = nullptr;

    if (f.fim == nullptr)
    {
        f.fim = novo;
        f.comeco = novo;
    }
    else
    {
        f.fim->eloP = novo;
        novo->eloA = f.fim;
        f.fim = novo;
    }
    return true;
}

// Consulta o primeiro elemento sem removê-lo (o "topo" da fila).
template <typename T>
bool top(const FilaDinamica<T> &f, T &valor)
{
    if (Vazia(f))
        return false;
    valor = f.comeco->info;
    return true;
}

// Percorre a fila do começo para o fim sem alterá-la, só mostrando os elementos.
template <typename T>
void mostrar(const FilaDinamica<T> &f)
{
    NoFila<T> *aux = f.comeco;

    while (aux != nullptr)
    {
        std::cout << aux->info << " ";
        aux = aux->eloP;
    }
}

template <typename T>
bool dequeue(FilaDinamica<T> &f, T &valor)
{
    if (Vazia(f))
        return false;

    NoFila<T> *aux = f.comeco;
    valor = aux->info;

    f.comeco = aux->eloP;
    if (f.comeco != nullptr)
        f.comeco->eloA = nullptr;
    else
        f.fim = nullptr; // a fila ficou vazia

    delete aux;
    return true;
}

template <typename T>
void liberar(FilaDinamica<T> &f)
{
    NoFila<T> *aux = f.comeco;
    while (aux != nullptr)
    {
        NoFila<T> *prox = aux->eloP;
        delete aux;
        aux = prox;
    }
    f.comeco = nullptr;
    f.fim = nullptr;
}

#endif // FILA_DINAMICA_HPP
