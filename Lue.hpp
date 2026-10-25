/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 */

#ifndef LUE_HPP
#define LUE_HPP

#include <iostream>
#include <string>
using namespace std;

template <typename T>
struct NoLue
{
    T info;       // valor guardado no nó (na lista de variáveis, fica reservado para o valor)
    string label; // rótulo do nó (na lista de variáveis, guarda o nome da variável)
    NoLue<T> *elo;
};

template <typename T>
struct Lue
{
    NoLue<T> *comeco;
    NoLue<T> *fim;
};

template <typename T>
void inicializar(Lue<T> &l)
{
    l.comeco = NULL;
    l.fim = NULL;
}

template <typename T>
bool Vazia(Lue<T> l)
{
    return (l.comeco == NULL) ? true : false;
}

template <typename T>
bool inserirFinal(Lue<T> &l, T valor, const string &label = "")
{
    NoLue<T> *novo = new NoLue<T>;

    if (novo == NULL)
        return false;
    novo->info = valor;
    novo->label = label;
    novo->elo = NULL;

    if (l.comeco == NULL)
    {
        l.comeco = novo;
        l.fim = novo;
    }
    else
    {
        l.fim->elo = novo;
        l.fim = novo;
    }
    return true;
}

template <typename T>
bool inserir(Lue<T> &l, T valor, const string &label = "")
{
    NoLue<T> *novo = new NoLue<T>;

    if (novo == NULL)
        return false;
    novo->info = valor;
    novo->label = label;
    novo->elo = NULL;

    if (l.comeco == NULL)
    {
        l.comeco = novo;
        l.fim = novo;
    }
    else if (valor > l.fim->info)
    {
        l.fim->elo = novo;
        l.fim = novo;
    }
    else if (valor < l.comeco->info)
    {
        novo->elo = l.comeco;
        l.comeco = novo;
    }
    else
    {
        NoLue<T> *ant = l.comeco;
        NoLue<T> *prox = ant->elo;
        while (prox != NULL)
        {
            if (ant->info < valor && valor < prox->info)
            {
                ant->elo = novo;
                novo->elo = prox;
                break;
            }
            ant = prox;
            prox = ant->elo;
        }
    }
    return true;
}

template <typename T>
bool pesquisar(Lue<T> l, T valor)
{
    NoLue<T> *aux = l.comeco;

    while (aux != NULL)
    {
        if (aux->info == valor)
            return true;
        aux = aux->elo;
    }
    return false;
}

// Procura pelo rótulo do nó. Comparar o label (e não o info) é o que permite
// usar a lista de variáveis, onde o nome fica no label e o info é o valor.
template <typename T>
bool pesquisarLabel(const Lue<T> &l, const string &label)
{
    NoLue<T> *aux = l.comeco;

    while (aux != NULL)
    {
        if (aux->label == label)
            return true;
        aux = aux->elo;
    }
    return false;
}

template <typename T>
bool retirarPosicao(Lue<T> &l, int pos)
{
    if (pos < 1)
        return false;

    int cont = 1;
    NoLue<T> *ant = NULL, *aux = l.comeco;

    while (aux != NULL && cont < pos)
    {
        cont++;
        ant = aux;
        aux = aux->elo;
    }
    if (aux == NULL)
        return false;

    if (ant == NULL)
    { // Retirar o primeiro elemento

        if (l.comeco == l.fim)
        { // Caso A
            l.comeco = NULL;
            l.fim = NULL;
        }
        else
        { // Caso B
            l.comeco = aux->elo;
        }
    }
    else
    {

        if (aux == l.fim)
        { // Caso C
            ant->elo = NULL;
            l.fim = ant;
        }
        else
        { // Caso D
            ant->elo = aux->elo;
        }
    }
    delete aux;
    return true;
}

template <typename T>
bool retirarValor(Lue<T> &l, T valor)
{
    int cont = 0;
    NoLue<T> *aux = l.comeco;
    while (aux != NULL)
    {
        cont++;
        if (aux->info == valor)
            return retirarPosicao(l, cont);
        aux = aux->elo;
    }
    return false;
}

template <typename T>
void mostrar(Lue<T> l)
{
    NoLue<T> *aux = l.comeco;

    while (aux != NULL)
    {
        cout << aux->info << " ";
        aux = aux->elo;
    }
}

template <typename T>
void liberar(Lue<T> &l)
{
    NoLue<T> *aux = l.comeco;
    NoLue<T> *prox;

    while (aux != NULL)
    {
        prox = aux->elo;
        delete aux;
        aux = prox;
    }
    l.comeco = NULL;
    l.fim = NULL;
}

template <typename T>
int comprimento(Lue<T> l)
{
    NoLue<T> *aux = l.comeco;
    int cont = 0;

    while (aux != NULL)
    {
        cont++;
        aux = aux->elo;
    }
    return cont;
}

template <typename T>
Lue<T> copiar(Lue<T> l)
{
    Lue<T> nova_lista;

    inicializar(nova_lista);
    NoLue<T> *aux = l.comeco;
    while (aux != NULL)
    {
        inserirFinal(nova_lista, aux->info, aux->label);
        aux = aux->elo;
    }
    return nova_lista;
}

#endif // LUE_HPP
