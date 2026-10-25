#include <iostream>
#include <string>

#include "calculadora.hpp"
#include "FilaDinamica.hpp"
#include "FilaEstatica.hpp"
#include "PilhaDinamica.hpp"
#include "PilhaEstatica.hpp"
#include "Lue.hpp"

using namespace std;

int main()
{
  string expressao;

  cout << "Digite a expressao:" << endl;
  getline(cin, expressao);

  FilaDinamica<char> fila_guarda_expressao;
  inicializar(fila_guarda_expressao);

  if (!inserir_expressao_na_fila(fila_guarda_expressao, expressao)) // Insere, se retornar erro, ocorreu um erro ao tentar inserir
  {
    cout << "Erro: nao foi possivel inserir a expressao na fila." << endl;
    return 1;
  }

  // após ter pego a expressão com sucesso, podemos começar a converter para a notação polonesa invertida ###
  // um ponto a ser levantando, essa variavel pilha_operadores. Pode ser criada na função converter_para_notacao_polonesa, porém, queremos poder escolher se é dinamica ou estática, então irá ficar na main para facilitar isso
  FilaDinamica<string> fila_notacao_polonesa_invertida; // fila que vai guardar na ordem da notação polonesa (variáveis podem ter letras e números)
  inicializar(fila_notacao_polonesa_invertida);

  PilhaDinamica<char> pilha_operadores; // pilha que vai guardar os operadores (ex: +, -, *)
  inicializar(pilha_operadores);

  Lue<string> lista_guarda_variaveis; // vai guardas as variaveis
  inicializar(lista_guarda_variaveis);

  if (!converter_para_notacao_polonesa(fila_guarda_expressao, fila_notacao_polonesa_invertida, pilha_operadores, lista_guarda_variaveis))
  {
    cout << "Ocorreu um erro ao tentar converter" << endl;
    return 1;
  }

  // cada variavel da expressao precisa de um valor para o calculo
  setar_variaveis(lista_guarda_variaveis);

  PilhaDinamica<double> pilha_valores; // pilha que vai guardar os operandos já resolvidos
  inicializar(pilha_valores);

  // a fila é consumida no cálculo, então ela não é mais impressa depois
  double resultado;
  if (!calcular_expressao(fila_notacao_polonesa_invertida, pilha_valores, lista_guarda_variaveis, resultado))
  {
    cout << "Ocorreu um erro ao tentar calcular a expressao" << endl;
    return 1;
  }

  cout << "Resultado: " << resultado << endl;

  return 0;
}