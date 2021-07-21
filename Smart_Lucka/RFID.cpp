#include <Arduino.h>
#include "definicoes_sistema.h"
#include "comunicacao.h"
#include <EEPROM.h> // We are going to read and write Tag's UIDs from/to EEPROM
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>
#include "RFID.h"

RFID::RFID(){}
MFRC522 mfrc522(10, 9);

void RFID::setup(){
    SPI.begin(); // MFRC522 Hardware uses SPI protocol
    Serial.begin(9600);
    mfrc522.PCD_Init(); // Initialize MFRC522 Hardware
    RFID::match = false; // inicializa variavel como false
    RFID::replaceMaster = false;
    RFID::ADM_ON = false;
    Serial.println("Inicializando o RFID");
    Serial.println("Deseja Configurar um Novo Cartao Mestre?: ");
    Serial.println("Se sim, apresente um cartao nos proximos 5s...");
    for (int i=5;i>0;i--){
      Serial.print(i);
      Serial.println("...");
      delay(1000);
      if (RFID::le_cartao()!=NENHUM_EVENTO){
          RFID::replaceMaster = true;
        }
      }
    if (RFID::replaceMaster){
      Serial.println("Cartao encontrado! Cadastrando como mestre...");
      Serial.print("Cartao: ");
      for (uint8_t j = 0; j < 4; j++ ) { // Loop 4 times
        EEPROM.write(2+j,RFID::readCard[j]); // Write scanned Tag's UID to EEPROM, start from address 3
      }
      for (uint8_t k = 0; k < 4; k++ ) { // Loop 4 times
        Serial.print(RFID::readCard[k]); // Write scanned Tag's UID to EEPROM, start from address 3
        Serial.println("");
      }
      EEPROM.write(1, 143); // Write to EEPROM we defined Master Card.  
      delay(2000);
    }  
    
    else{
      Serial.println("OK! voce decidiu manter o cartao mestre da última sessão");
      }
    for ( uint8_t i = 0; i < 4; i++ ) {          // Read Master Card's UID from EEPROM
      RFID::masterCard[i] = EEPROM.read(2+i);    // Write it to masterCard
    }
    Serial.println("Cartao mestre: ");
    for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
      Serial.print(RFID::masterCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
    }
    Serial.println("");
}


// METODO DE LEITURA DO CARTAO
/*
int RFID::le_cartao()
metodo publico usado para verificar a presenca de um cartao RFID
esse metodo funciona essencialmente como o obter_evento() da maquina de estados, 
retornando o evento apropriado de acordo com a leitura do cartao; 
*/

int RFID::le_cartao() {
  // funcao que verifica se ha um cartao proximo e, se houver, retorna seu tipo
  if (!mfrc522.PICC_IsNewCardPresent()) { // se nao leu nenhuma Tag, retorne NENHUM_EVENTO
    return NENHUM_EVENTO;
  }
  if (!mfrc522.PICC_ReadCardSerial()) { //Since a Tag placed get Serial and continue
    return NENHUM_EVENTO;
  }
  // se o programa chegou ate aqui entao temos um cartao valido, portanto vamos armazena-lo
  // na propriedade .readCard da classe
  for (int i=0;i<4;i++) {
    RFID::readCard[i] = mfrc522.uid.uidByte[i]; // transcrevendo os valores no RFID
  }
  mfrc522.PICC_HaltA(); // termina a leitura

  if(RFID::Mestre(RFID::readCard)){ // checa pra ver ser o cartao eh um cartao ADM
    if (RFID::ADM_ON){
      RFID::ADM_ON = false;
      Serial.println("DEIXANDO O MODO ADM!");
      delay(500);
      }
    else{
      Serial.println("ENTRANDO NO MODO ADM!");
      RFID::ADM_ON = true;
      }
    Serial.println("Cartao Lido: ");
    for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
      Serial.print(RFID::readCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
    }
    Serial.println("");
    return CARTAO_ADM;
  }
  else if (RFID::procura_ID(RFID::readCard)){ // se nao eh cartao ADM, checa pra ver se ao menos eh cartao valido
    if (RFID::ADM_ON){
      Serial.print("REMOVENDO PERMISSAO DO CARTAO: ");
      for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
        Serial.print(RFID::readCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
      }
    Serial.println("");
    }
    else{
      Serial.println("ACESSO PERMITIDO!");
      Serial.print("CARTAO UTILIZADO: ");
      for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
        Serial.print(RFID::readCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
      }
      Serial.println("");
    }
    return CARTAO_GRANTED; 
  }
  else{
    if (RFID::ADM_ON){
      Serial.print("ADICIONANDO PERMISSAO DO CARTAO: ");
      for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
        Serial.print(RFID::readCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
      }
    Serial.println("");
    }
    else{
      Serial.println("ACESSO NEGADO!");
      Serial.print("CARTAO UTILIZADO: ");
      for (uint8_t z = 0; z < 4; z++ ) { // Loop 4 times
        Serial.print(RFID::readCard[z]); // Write scanned Tag's UID to EEPROM, start from address 3
      }
      Serial.println("");
    }
    return CARTAO_DENIED; // se chegou ate aqui entao o cartao eh invalido
  }
}

// METODOS AUXILIARES PRIVADOS
/*o metodo auxiliar privado compara_cartoes() verifica se dois arrays sao iguais e eh usado
para verificar leituras do RFID com os valores armazenados na EEPROM
*/
boolean RFID::compara_cartoes(byte a[], byte b[]){
  if (a[0]!=0) {          // Make sure there is something in the array first    RFID.match = true;       // Assume they match at first
  RFID::match=true;
  }
  for (int k=0;k<4;k++) {   // Loop 4 times
    if (a[k]!=b[k])     // IF a != b then set match = false, one fails, all fail
      RFID::match = false;
  }
  if (RFID::match) {      // Check to see if if match is still true
    return true;      // Return true
  }
  else{
    return false;       // Return false
  }
}


/*o metodo auxiliar privado Mestre() eh utilizado em conjunto com o metodo 
 * auxiliar compara_cartoes() para verificar se o cartao lido eh do tipo mestre*/
boolean RFID::Mestre(byte test[]){
  if (RFID::compara_cartoes(test,RFID::masterCard))
    return true;
  else
    return false;
}

/* o metodo auxiliar privado procura_ID verifica se o ID do cartao encontra-se na EEPROM
 * retorna true se este eh o caso e, caso contrario, retorna false
*/
boolean RFID::procura_ID(byte test[]) {
  uint8_t count = EEPROM.read(0);     // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i <= count; i++) {    // Loop once for each EEPROM entry
    RFID::EEPROM_ID(i);          // Read an ID from EEPROM, it is stored in storedCard[4]
    if (RFID::compara_cartoes(test,RFID::storedCard)) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;
    }
  }
  return false;
}

/* o metodo auxiliar privado EEPROM_ID atualiza o atributo stored_Card que representa um cartao
 *  qualquer presenta na EEPROM; o metodo eh usado para verificar se um cartao lido esta na EEPROM
*/

void RFID::EEPROM_ID(int numero) {
  uint8_t start = (numero*4) + 2;    // Figure out starting position
  for ( uint8_t i = 0; i < 4; i++ ) {     // Loop 4 times to get the 4 Bytes
    RFID::storedCard[i] = EEPROM.read(start + i);   // Assign values read from EEPROM to array
  }
}

// METODOS DO MODO ADM
/* o metodo publico deletaID, bem, deleta um ID da EEPROM, obviamente
 * só é chamado quando a maquina de estados recebe a sequência de eventos MODO_ADM -> CARTAO_GRANTED
*/
void RFID::deletaID(byte a[]) {
  uint8_t num = EEPROM.read(0);                       // posição 0 guarda a quantidade de cartoes ID
  uint8_t slot;                                       // variavel utilizada pra achar a pos inicial do cartao ID
  uint8_t start;                                      // = (num*4) + 6; // descobre onde fica o prox cartao
  uint8_t looping;                                    // numero de loops
  uint8_t j;
  slot = RFID::findIDSLOT(a);                         // chama a funcao para encontrar o slot do cartao
  start = (slot*4)+2;
  looping = ((num-slot)*4);
  num--;                                              // Decrement the counter by one
  EEPROM.write(0,num);                                // Write the new count to the counter
  for (j=0;j<looping;j++) {                           // Loop the card shift times
    EEPROM.write(start+j,EEPROM.read(start+4+j));     // Shift the array values to 4 places earlier in the EEPROM
  }
  for (uint8_t k=0;k<4;k++) {                         // Shifting loop
    EEPROM.write(start+j+k,0);
  }
  delay(1000);
  return;
}

/* o metodo publico escreveID, bem, escreve um ID da EEPROM, obviamente
 * só é chamado quando a maquina de estados recebe a sequência de eventos MODO_ADM -> CARTAO_DENIED
*/
void RFID::escreveID(byte a[]) {
  uint8_t num = EEPROM.read(0);                       // Get the numer of used spaces, position 0 stores the number of ID cards
  uint8_t start = (num*4) + 6;                        // Figure out where the next slot starts
  num++;                                              // Increment the counter by one
  EEPROM.write(0,num);                                // Write the new count to the counter
  for (uint8_t j=0;j<4;j++) {                         // Loop 4 times
    EEPROM.write(start+j,a[j]);                       // Write the array values to EEPROM in the right position
  }
  delay(1000);
  return;
}

/*
 * o metodo privado findIDSLOT
*/

int RFID::findIDSLOT(byte a[]) {
  uint8_t count = EEPROM.read(0);       // Read the first Byte of EEPROM that
  for (uint8_t i=1;i<=count;i++) {    // Loop once for each EEPROM entry
    RFID::EEPROM_ID(i);                // Read an ID from EEPROM, it is stored in storedCard[4]
    if (RFID::compara_cartoes(a,RFID::storedCard)) {   // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i;         // The slot number of the card
      break;          // Stop looking we found it
    }
  }
}
