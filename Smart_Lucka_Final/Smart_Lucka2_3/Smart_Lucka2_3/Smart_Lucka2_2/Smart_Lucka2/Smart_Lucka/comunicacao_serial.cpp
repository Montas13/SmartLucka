#include <Arduino.h>

#include "definicoes_sistema.h"
#include "comunicacao_serial.h"

ComunicacaoSerial::ComunicacaoSerial()
{
}

/************************
 ComunicacaoSerial::notificar
 Envia mensagem para a Central
 entradas
   texto : texto para envio para Central
 saidas
   nenhuma
*************************/
void ComunicacaoSerial::notificar(char* texto)
{
    Serial.print("Comunicacao com a Central: ");
    Serial.println(texto);
}
