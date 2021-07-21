#ifndef RFID_INCLUDED
#define RFID_INCLUDED

class RFID{
  public:
  RFID();
  void setup();
  int le_cartao(); // metodo que le o cartao RFID e retorna o EVENTO apropriado
  int modo_ADM(); // metodo que realiza as operacoes do modo ADM
  void deletaID(byte a[]);
  void escreveID(byte a[]);
  byte readCard[4];             // Stores scanned ID read from RFID Module
  int estado;
  
  private:
  // metodos
  boolean compara_cartoes(byte a[], byte b[]); // metodo auxiliar que compara duas listas
  boolean Mestre(byte test[]);                 // metodo auxiliar que verifica se o cartao eh do tipo mestre
  boolean procura_ID(byte test[]);             // metodo auxiliar que procura o ID do cartao lido na EEPROM
  void EEPROM_ID(int numero);                  // metodo auxiliar que retorna o valor de um ID na EEPROM  
  int findIDSLOT(byte a[]);                    // metodo auxiliar que encontra ID na EEPROM
  // atributos
  byte storedCard[4];                          // Stores an ID read from EEPROM
  byte masterCard[4];                          // Stores master card's ID read from EEPROM
  boolean match;                               // initialize card match to false
  boolean replaceMaster;
  boolean ADM_ON; 
  };

#endif // RFID_INCLUDED
