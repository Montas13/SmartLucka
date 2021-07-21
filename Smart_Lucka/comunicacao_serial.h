#ifndef COMUNICACAO_SERIAL_H
#define COMUNICACAO_SERIAL_H

#include "comunicacao.h"

class ComunicacaoSerial: public Comunicacao {
  public:
  ComunicacaoSerial();
  void notificar(char* texto);
};

#endif // COMUNICACAO_SERIAL_H
