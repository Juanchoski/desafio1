#include <Adafruit_LiquidCrystal.h>

int pinA = A0;
int valor = 0;
int pulsadorI = 2;
int pulsadorF = 3;
Adafruit_LiquidCrystal lcd(0);

void setup (){
  pinMode(pulsadorI,INPUT);
  pinMode(pulsadorF,INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido");
} 

void loop(){
  valor = analogRead(pinA);
  Serial.println(valor);
}
