#include <Arduino.h>
#include <Servo.h>
#include "definicoes_sistema.h"
#include "_servo.h"


// portas dos Servo
const int servoPin = 8;

_Servo::_Servo(){} 
Servo my_servo;

// funcao que inicializa o Servo
void _Servo::setup(){
    my_servo.attach(servoPin);
    my_servo.write(10);
}

// funcao que sinaliza a operacao do servo
void _Servo::acionar(int modo){
    switch (modo){
        case ABRIR:
            my_servo.write(90);
            break;
        case FECHAR:
            my_servo.write(10);
            break;
    }
}
