#include <Adafruit_LiquidCrystal.h>
int pinA = A0;
int valor = 0;
int pulsadorI = 2;
int pulsadorF = 3;
int *arregloV; 
int muestras = 0;
Adafruit_LiquidCrystal lcd(0);

void setup() {
  pinMode(pulsadorI, INPUT);
  pinMode(pulsadorF, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido");
  muestras = 100; 
  crearArreglo(muestras);
  almacenar();
}

void loop() {
  valor = analogRead(pinA);
  Serial.println(valor);
}

void crearArreglo(int T) {  
  arregloV = new int[T];
}

void almacenar() {
  for (int v = 0; v < muestras; v++) {
    arregloV[i] = analogRead(pinA); 
    Serial.print("Valor "); 
    Serial.print(v); 
    Serial.print(": "); 
    Serial.println(arregloV[v]);
    delay(100); 
  }
}
