#ifndef DEFINICOES_SISTEMA_H_INCLUDED
#define DEFINICOES_SISTEMA_H_INCLUDED

#define true  1
#define false 0

#define MAX_TAREFAS 3

#define NUM_ESTADOS 3
#define NUM_EVENTOS 4

// ESTADOS
#define ESPERA           0
#define MODO_ADM         1
#define MODO_LEITURA     2

// EVENTOS
#define NENHUM_EVENTO   -1
#define CARTAO_GRANTED   0
#define CARTAO_DENIED    1
#define CARTAO_ADM       2

// ACOES
#define NENHUMA_ACAO -1
#define A01  0
#define A02  1
#define A03  2
#define A04  3
#define A05  4
#define A06  5

// CASOS LEDS
#define GRANTED 1
#define DENIED 2
#define ADM 3

// CASOS SERVO
#define ABRIR 0
#define FECHAR 1

// CASOS RFID
#define REMOVER 0
#define ADICIONAR 1

#endif // DEFINICOES_SISTEMA_H_INCLUDED
