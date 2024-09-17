#include <Adafruit_LiquidCrystal.h>

int pinEntrada = A0;       // Pin de entrada de la señal
int botonInicio = 2;       // Boton de inicio
int botonFin = 3;          // Boton de fin
int valor = 0;             // Valor analogico leido
int valorMaximo = 0;       // Maximo valor detectado
int valorMinimo = 1023;    // Minimo valor detectado
int estadoBotonInicio = 0; // Estado del boton de inicio
int estadoBotonFin = 0;    // Estado del boton de fin
int frecuencia = 0;        // Frecuencia en Hz
Adafruit_LiquidCrystal lcd(0);  // Inicializacion del LCD

float tiempoInicio = 0;    // Tiempo inicial para la medicion
float tiempoFin = 0;       // Tiempo final para la medicion
int estadoSenial = LOW;     // Estado actual de la senal
int ultimoEstadoSenial = LOW; // Estado anterior de la senal
int ciclos = 0;            // Contador de ciclos de la senal
float amplitud = 0;        // Amplitud de la senal
bool leyendo = false;      // Variable para saber si está leyendo datos

void setup() {
  pinMode(botonInicio, INPUT);
  pinMode(botonFin, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido");
}

void loop() {
  estadoBotonInicio = digitalRead(botonInicio);
  estadoBotonFin = digitalRead(botonFin);

  if (!leyendo && estadoBotonInicio == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Leyendo...");
    tiempoInicio = millis();  
    ciclos = 0;
    valorMaximo = 0;
    valorMinimo = 1023;
    leyendo = true;
    Serial.println("Midiendo...");
  }

  if (leyendo) {
    valor = analogRead(pinEntrada);

    if (valor > valorMaximo) valorMaximo = valor;
    if (valor < valorMinimo) valorMinimo = valor;

    estadoSenial = (valor > (valorMinimo + valorMaximo) / 2) ? HIGH : LOW;

    if (estadoSenial != ultimoEstadoSenial && estadoSenial == HIGH) {
      ciclos++;
    }

    ultimoEstadoSenial = estadoSenial;
  }

  if (leyendo && estadoBotonFin == HIGH) {
    tiempoFin = millis();  
    leyendo = false;

    float tiempoTranscurrido = (tiempoFin - tiempoInicio) / 1000.0;

    frecuencia = ciclos / tiempoTranscurrido;

    amplitud = ((valorMaximo) / 1000.0) * 10.0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Freq:");
    lcd.print(frecuencia);
    lcd.print(" Hz");

    lcd.setCursor(0, 1);
    lcd.print("Amp:");
    lcd.print(amplitud, 2);
    lcd.print(" V");

    Serial.print("Frecuencia: ");
    Serial.print(frecuencia);
    Serial.print(" Hz, Amplitud: ");
    Serial.print(amplitud);
    Serial.println(" V");
  }
}
