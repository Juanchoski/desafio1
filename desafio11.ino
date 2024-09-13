#include <LiquidCrystal_I2C.h>
int pinA = A0;
int valor = 0;
int pulsadorI = 2;
int pulsadorF = 3;
void setup (){
  pinMode(pulsadorI,IMPUT);
  pinMode(pulsadorF,IMPUT);
  Serial.begin(9600);
} 
void loop(){
  valor = analogRead(pinA);
  Serial.println(valor);
}
