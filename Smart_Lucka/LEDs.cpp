#include <Arduino.h>
#include "definicoes_sistema.h"
#include "LEDs.h"


// portas dos LEDs
const int yellowLedPin = 5;
const int greenLedPin = 6;
const int redLedPin = 7;

LEDs::LEDs(){} 

// funcao que inicializa os LEDs
void LEDs::setup(){
    pinMode(yellowLedPin,OUTPUT);
    pinMode(greenLedPin,OUTPUT);
    pinMode(redLedPin,OUTPUT);
    digitalWrite(yellowLedPin,LOW);
    digitalWrite(greenLedPin,LOW);
    digitalWrite(redLedPin,LOW);
}

// funcao que sinaliza a operacao da maquina de estados
void LEDs::acionar(int modo){
    switch (modo){
        case ADM:
            digitalWrite(yellowLedPin,HIGH);
            digitalWrite(greenLedPin,LOW);
            digitalWrite(redLedPin,LOW);
            break;
        case GRANTED:
            digitalWrite(yellowLedPin,LOW);
            digitalWrite(greenLedPin,HIGH);
            digitalWrite(redLedPin,LOW);
            break;
        case DENIED:
            digitalWrite(yellowLedPin,LOW);
            digitalWrite(greenLedPin,LOW);
            digitalWrite(redLedPin,HIGH);
            break;
        case ESPERA:
            digitalWrite(yellowLedPin,LOW);
            digitalWrite(greenLedPin,LOW);
            digitalWrite(redLedPin,LOW);
            break;
    }
}

void LEDs::cycle(int modo){
  // funcao que faz o ciclo de alguns leds para sinalizar certos eventos
  if (modo == REMOVER){
    digitalWrite(yellowLedPin,LOW);
    digitalWrite(greenLedPin,LOW);
    digitalWrite(redLedPin,HIGH);
    delay(200);
    digitalWrite(redLedPin,LOW);
    delay(200);
    digitalWrite(redLedPin,HIGH);
    delay(200);
    digitalWrite(redLedPin,LOW);
    delay(200);
    digitalWrite(redLedPin,HIGH);
    delay(200);
    digitalWrite(redLedPin,LOW);
    delay(200);
    }
  else{
    digitalWrite(yellowLedPin,LOW);
    digitalWrite(redLedPin,LOW);
    digitalWrite(greenLedPin,HIGH);
    delay(200);
    digitalWrite(greenLedPin,LOW);
    delay(200);
    digitalWrite(greenLedPin,HIGH);
    delay(200);
    digitalWrite(greenLedPin,LOW);
    delay(200);
    digitalWrite(greenLedPin,HIGH);
    delay(200);
    digitalWrite(greenLedPin,LOW);
    delay(200);
    }
  }
