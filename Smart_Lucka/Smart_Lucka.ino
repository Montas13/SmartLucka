#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <MySQL_Encrypt_Sha1.h>
#include <MySQL_Packet.h>

/*
  CONTROLE DE ACESSO SMART LUCKA
  GUILHERME RODRIGUES MONTEIRO - 10706103
  JOAO RODRIGO WINDISCH OLENSCKI - 10773224
  LUCA RODRIGUES MIGUEL - 10705655
  LUI DAMIANCI FERREIRA - 10770579
*/

#include "definicoes_sistema.h" // manter
#include "comunicacao_serial.h" // analisar
#include "task_switcher.h" // manter
#include "LEDs.h" // manter
#include "_servo.h" // manter
#include "RFID.h"

#define TASK_INTERVAL1 500
#define TASK_INTERVAL2 500
#define TASK_INTERVAL3 1000

/***********************************************************************
 Componentes
 ***********************************************************************/

_Servo servo; // manter
RFID rfid; // manter
LEDs led; // manter
/***********************************************************************
 Estaticos
 ***********************************************************************/
int codigoEvento = NENHUM_EVENTO;
int eventoInterno = NENHUM_EVENTO;
int estado = ESPERA;
int codigoAcao;
int acao_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];
int proximo_estado_matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS];

/************************************************************************
 executarAcao
 Executa uma acao
 Parametros de entrada:
    (int) codigo da acao a ser executada
 Retorno: (int) codigo do evento interno ou NENHUM_EVENTO
*************************************************************************/
int executarAcao(int codigoAcao)
{
    int retval;

    retval = NENHUM_EVENTO;
    if (codigoAcao == NENHUMA_ACAO)
        return retval;

    switch(codigoAcao)
    {
    case A01: // ACCESS GRANTED: usuario apresenta cartao que possui acesso; basta sinalizar OK e abrir a porta
        led.acionar(GRANTED); // aciona LED verde de acesso permitido
        servo.acionar(ABRIR); // aciona o servo para abrir a porta
        delay(3000); // aguarda 3 seg
        servo.acionar(FECHAR); // aciona o servo para fechar a porta
        led.acionar(ESPERA); // desliga os LEDs
        break;
    case A02: // ACCESS DENIED: usuario apresenta cartao que nao possui acesso; basta acender LED vermelho
        led.acionar(DENIED); // aciona LED vermelho de acesso negado
        delay(3000); // aguarda 3 seg
        led.acionar(ESPERA); // aciona o servo para abrir a porta
        break;
    case A03: // MODO ADM: usuario apresenta cartao ADM, estado mudará pra MODO ADM e acenderá terceiro LED
        led.acionar(ADM);
        break;
    case A04: // REMOVER CARTAO: usuario no modo ADM apresenta cartao valido e, portanto, o mesmo será removido da base
        led.cycle(REMOVER);
        rfid.deletaID(rfid.readCard);
        led.acionar(ADM);
        break;
    case A05: // ADICIONAR CARTAO: usuario no modo ADM apresenta cartao invalido e, portanto, o mesmo será adicionado a base
        led.cycle(ADICIONAR);
        rfid.escreveID(rfid.readCard);
        led.acionar(ADM);
        break;
    case A06: // SAIR DO MODO ADM: usuario no modo ADM apresenta novamente um cartao ADM e, portanto, sai do modo ADM
        led.acionar(ESPERA);
        break;
        
    return retval;
}
} // executarAcao

/************************************************************************
 iniciaMaquina de Estados
 Carrega a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaMaquinaEstados()
{
  int i;
  int j;

  for (i=0; i < NUM_ESTADOS; i++) {
    for (j=0; j < NUM_EVENTOS; j++) {
       acao_matrizTransicaoEstados[i][j] = NENHUMA_ACAO;
       proximo_estado_matrizTransicaoEstados[i][j] = i;
    }
  }
  proximo_estado_matrizTransicaoEstados[ESPERA][CARTAO_GRANTED] = ESPERA; // relacionado ao evento de apresentar um cartao valido no modo normal
  acao_matrizTransicaoEstados[ESPERA][CARTAO_GRANTED] = A01; // relacionado a abrir a porta

  proximo_estado_matrizTransicaoEstados[ESPERA][CARTAO_DENIED] = ESPERA; // relacionado ao evento de apresentar um cartao invalido no modo normal
  acao_matrizTransicaoEstados[ESPERA][CARTAO_DENIED] = A02; // relacionado a notificar acesso negado

  proximo_estado_matrizTransicaoEstados[ESPERA][CARTAO_ADM] = MODO_ADM; // relacionado a entrar no modo adm
  acao_matrizTransicaoEstados[ESPERA][CARTAO_ADM] = A03; // relacionado a notificar estado adm

  proximo_estado_matrizTransicaoEstados[MODO_ADM][CARTAO_GRANTED] = MODO_ADM; // relacionado a apresentar um cartao pra ser deletado
  acao_matrizTransicaoEstados[MODO_ADM][CARTAO_GRANTED] = A04; // relacionado a deletar cartao

  proximo_estado_matrizTransicaoEstados[MODO_ADM][CARTAO_DENIED] = MODO_ADM; // relacionado a adicionar um cartao a EEPROM
  acao_matrizTransicaoEstados[MODO_ADM][CARTAO_DENIED] = A05; // relacionado a escrever cartao na EEPROM

  proximo_estado_matrizTransicaoEstados[MODO_ADM][CARTAO_ADM] = ESPERA; // relacionado a sair do modo ADM
  acao_matrizTransicaoEstados[MODO_ADM][CARTAO_ADM] = A06; // relacionado a notificar saída do modo ADM

} // initStateMachine

/************************************************************************
 iniciaSistema
 Inicia o sistema ...
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void iniciaSistema()
{
   iniciaMaquinaEstados();
} // initSystem

/************************************************************************
 obterAcao
 Obtem uma acao da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo da acao
*************************************************************************/
int obterAcao(int estado, int codigoEvento) {
  return acao_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao


/************************************************************************
 obterProximoEstado
 Obtem o proximo estado da Matriz de transicao de estados
 Parametros de entrada: estado (int)
                        evento (int)
 Retorno: codigo do estado
*************************************************************************/
int obterProximoEstado(int estado, int codigoEvento) {
  return proximo_estado_matrizTransicaoEstados[estado][codigoEvento];
} // obterAcao


/***********************************************************************
 Tasks
 ***********************************************************************/

/************************************************************************
 taskMaqEstados
 Task principal de controle que executa a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void taskMaqEstados() {
  if (eventoInterno != NENHUM_EVENTO) {
      codigoEvento = eventoInterno;
  }
  if (codigoEvento != NENHUM_EVENTO)
  {
      codigoAcao = obterAcao(estado, codigoEvento);
      estado = obterProximoEstado(estado, codigoEvento);
      eventoInterno = executarAcao(codigoAcao);
      Serial.print("Estado: ");
      Serial.print(estado);
      Serial.print(" Evento: ");
      Serial.print(codigoEvento);
      Serial.print(" Acao: ");
      Serial.println(codigoAcao);
  }
}

/************************************************************************
 taskObterEvento
 Task que faz pooling de eventos
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/
void taskObterEvento() {
  codigoEvento = rfid.le_cartao();
}

/************************************************************************
 Main
 Setup e Loop principal de controle que executa a maquina de estados
 Parametros de entrada: nenhum
 Retorno: nenhum
*************************************************************************/

void setup() {
  Serial.begin(9600);

  // configure tasks
  TaskController.createTask(&taskMaqEstados, TASK_INTERVAL1);
  TaskController.createTask(&taskObterEvento, TASK_INTERVAL2);
  
  // set up timer interrupt 
  TaskController.begin(1000); // tick @1ms (1000 us)

  iniciaSistema();
  Serial.println("SmartLucka iniciado");
  led.setup();
  Serial.println("Led Iniciado");
  servo.setup();
  Serial.println("Servo Iniciado");
  rfid.setup();
  Serial.println("RFID Iniciado");
} // setup

void loop() {
  TaskController.runCurrentTask();
} // loop
