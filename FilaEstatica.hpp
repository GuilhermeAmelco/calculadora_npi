/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 */

#ifndef FILA_ESTATICA_HPP
#define FILA_ESTATICA_HPP

#include <iostream>

#ifndef TAM
#define TAM 50
#endif // TAM

// Fila circular: reaproveita as posições liberadas no início do vetor,
// evitando o desperdício da fila linear simples.
template <typename T>
struct FilaEstatica
{
    T vetor[TAM];
    int comeco, fim;
    int qtd;
};

template <typename T>
void inicializar(FilaEstatica<T> &f)
{
    f.comeco = 0;
    f.fim = -1;
    f.qtd = 0;
}

template <typename T>
bool Vazia(const FilaEstatica<T> &f)
{
    return f.qtd == 0;
}

template <typename T>
bool Cheia(const FilaEstatica<T> &f)
{
    return f.qtd == TAM;
}

template <typename T>
bool queue(FilaEstatica<T> &f, T valor)
{
    if (Cheia(f))
        return false;
    f.fim = (f.fim + 1) % TAM;
    f.vetor[f.fim] = valor;
    f.qtd++;
    return true;
}

// Consulta o primeiro elemento sem removê-lo (o "topo" da fila).
template <typename T>
bool top(const FilaEstatica<T> &f, T &valor)
{
    if (Vazia(f))
        return false;
    valor = f.vetor[f.comeco];
    return true;
}

// Percorre a fila do começo para o fim sem alterá-la, só mostrando os elementos.
template <typename T>
void mostrar(const FilaEstatica<T> &f)
{
    int pos = f.comeco;

    for (int i = 0; i < f.qtd; i++)
    {
        std::cout << f.vetor[pos] << " ";
        pos = (pos + 1) % TAM; // a fila é circular
    }
}

template <typename T>
bool dequeue(FilaEstatica<T> &f, T &valor)
{
    if (Vazia(f))
        return false;
    valor = f.vetor[f.comeco];
    f.comeco = (f.comeco + 1) % TAM;
    f.qtd--;
    return true;
}

// Não há memória dinâmica a desalocar: basta reinicializar a fila.
template <typename T>
void liberar(FilaEstatica<T> &f)
{
    inicializar(f);
}

#endif // FILA_ESTATICA_HPP
