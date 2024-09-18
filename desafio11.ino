#include <Adafruit_LiquidCrystal.h>

int pinEntrada = A0;       
int botonInicio = 2;       
int botonFin = 3;          
int valor = 0;             
int valorAnterior = 0;     
int valorMaximo = 0;       
int valorMinimo = 1023;    
int estadoBotonInicio = 0; 
int estadoBotonFin = 0;    
bool maxArreglo = false;
unsigned int dimension = 10; 
unsigned int posicionA = 0;   
float frecuencia = 0;       
Adafruit_LiquidCrystal lcd(0); 

int* grafica = new int[dimension];

unsigned long tiempoInicio = 0; 
unsigned long tiempoFin = 0;    
int estadoSenial = LOW;         
int ultimoEstadoSenial = LOW;   
int ciclos = 0;                 
float amplitud = 0;             
bool leyendo = false;           
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
    posicionA = 0;
    Serial.println("Midiendo...");
  }

  if (leyendo) {
    valor = analogRead(pinEntrada);
    
    int umbral = (valorMinimo + valorMaximo) / 2;
    estadoSenial = (valor > umbral) ? HIGH : LOW;

    if (estadoSenial == HIGH && ultimoEstadoSenial == LOW) {
      ciclos++;
    }
    ultimoEstadoSenial = estadoSenial;

    if (valor != valorAnterior) {
      if (posicionA < dimension) {
        valorAnterior = valor;
        grafica[posicionA] = valor;
        posicionA++;
      } else if (!maxArreglo) {
        maxArreglo = aumentoArreglo(grafica, dimension);
      } else {
        Serial.println("Memoria llena, se dejan de tomar valores de las ondas");
      }
    }

    if (valor > valorMaximo) valorMaximo = valor;
    if (valor < valorMinimo) valorMinimo = valor;
  }

  if (leyendo && estadoBotonFin == HIGH) {
    tiempoFin = millis();  
    leyendo = false;

    float tiempoTranscurrido = (tiempoFin - tiempoInicio) / 1000.0;
    frecuencia = ciclos / tiempoTranscurrido;
    amplitud = ((valorMaximo - valorMinimo) / 1000.0) * 5.0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Freq:");
    lcd.print(frecuencia);
    lcd.print(" Hz");

    lcd.setCursor(0, 1);
    lcd.print("Amp:");
    lcd.print(amplitud, 2);
    lcd.print(" V");

    tipoOnda(grafica, posicionA);
    
    if (esTriangular(grafica, posicionA)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Onda Triangular");
    }
    else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("onda desconocida");
  }

    posicionA = 0;
    maxArreglo = false;
    ciclos = 0;
    tiempoInicio = 0;
  }
}

void tipoOnda(int* grafica, unsigned int dimension) {
  bool esCuadrada = true;
  int valor1 = grafica[0];
  int valor2 = grafica[1];

  for (int i = 2; i < dimension; i++) {
    if ((i % 2 == 0 && grafica[i] != valor1) || (i % 2 != 0 && grafica[i] != valor2)) {
      esCuadrada = false;
      break;
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);

  if (esCuadrada) {
    lcd.print("Onda Cuadrada");
  } 
}

bool aumentoArreglo(int*& grafica, unsigned int &dimension) {
  if (dimension * 2 >= 300) {
    return false; 
  }

  int nuevoTamano = dimension * 2;
  int* G = new int[nuevoTamano];
  for (int a = 0; a < dimension; a++) {
    G[a] = grafica[a];
  }

  delete[] grafica;
  grafica = G;
  dimension = nuevoTamano;
  return true;
}

bool esTriangular(int* grafica, unsigned int dimension) {
  if (dimension < 3) return false;
  bool subiendo = true;
  int diferencia = grafica[1] - grafica[0];

  for (int t = 1; t < dimension - 1; t++) {
    int diferenciaActual = grafica[t + 1] - grafica[t];

    if (subiendo) {
      if (diferenciaActual < 0) {
        subiendo = false;
        diferencia = -diferencia;
      } else if (diferenciaActual != diferencia) {
        return false;
      }
    } else {
      if (diferenciaActual != -diferencia) {
        return false;
      }
    }
  }

  return true;
}
