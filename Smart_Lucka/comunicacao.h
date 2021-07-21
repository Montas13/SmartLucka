#ifndef COMUNICACAO_H_INCLUDED
#define COMUNICACAO_H_INCLUDED

class Comunicacao {
  public:
  /************************
   notificar
  Envia mensagem para a Central
  entradas
    texto : texto para envio para Central
  saidas
    nenhuma
  *************************/
  virtual void notificar(char* texto) = 0;
};

#endif // COMUNICACAO_H_INCLUDED
